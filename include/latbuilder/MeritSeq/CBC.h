// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LATBUILDER__MERIT_SEQ__CBC_H
#define LATBUILDER__MERIT_SEQ__CBC_H

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/LatDef.h"
#include "latbuilder/BridgeSeq.h"
#include "latbuilder/BridgeIteratorCached.h"
#include "latbuilder/LatSeq/CBC.h"

#include "latcommon/CoordinateSets.h"

#include <memory>

namespace LatBuilder { namespace MeritSeq {

/**
 * Sequence of lattice merit for CBC construction.
 *
 * This class is intended to be used with the following steps:
 * <ol>
 * <li> Instantiate.  At that point, the dimension is zero. </li>
 * <li> Iterate over the sequence of merit values and lattices for the next
 *      dimension with CBC::Seq::begin() and CBC::Seq::end(). </li>
 * <li> Select an element from the sequence with #select().  This increases the
 *      current dimension by one. </li>
 * <li> Repeat from step 2 until desired dimension is reached. </li>
 * </ol>
 */
template <LatType LAT, Compress COMPRESS, class PROJDEP, template <typename> class ACC>
class CBC
{
public:
   typedef WeightedFigureOfMerit<PROJDEP, ACC> FigureOfMerit;
   typedef typename Storage<LAT, COMPRESS>::MeritValue MeritValue;
   typedef LatBuilder::LatDef<LAT> LatDef;
   typedef MeritValue value_type;
   typedef decltype(std::declval<FigureOfMerit>().evaluator(std::declval<Storage<LAT, COMPRESS>>())) Evaluator;

   typedef LatCommon::CoordinateSets::AddCoordinate<LatCommon::CoordinateSets::FromRanges> Projections;

   /**
    * Constructor.
    *
    * \param storage     Storage configuration.
    * \param figure      Figure of merit instance to be used.
    */
   CBC(
         Storage<LAT, COMPRESS> storage,
         const FigureOfMerit& figure
         ):
      m_storage(std::move(storage)),
      m_figureOfMerit(std::move(figure)),
      m_eval(this->figureOfMerit().evaluator(this->storage())),
      m_baseLat(LatDef(this->storage().sizeParam())),
      m_baseMerit(this->storage().createMeritValue(0.0))
   {}

   /**
    * Resets the state of the CBC algorithm to dimension 0.
    */
   void reset()
   {
      m_baseLat = LatDef(storage().sizeParam());
      m_baseMerit = storage().createMeritValue(0.0);
   }

   /**
    * Returns the storage configuration instance.
    */
   const Storage<LAT, COMPRESS>& storage() const
   { return m_storage; }

   /**
    * Returns the figure of merit.
    */
   const FigureOfMerit& figureOfMerit() const
   { return m_figureOfMerit; }

   /**
    * Returns the figure of merit evaluator.
    */
   const Evaluator& evaluator() const
   { return m_eval; }

   /**
    * Returns the base lattice.
    *
    * This sequence extends on the lattice base by increasing its dimension by
    * one.
    *
    * \sa #baseMerit()
    */
   const LatDef& baseLat() const
   { return m_baseLat; }

   /**
    * Returns the base merit value.
    *
    * The merit values contained in this sequence are based on the value
    * returned by this function.
    *
    * \sa #baseLat()
    */
   const MeritValue& baseMerit() const
   { return m_baseMerit; }

   /**
    * Returns a sequence of coordinate sets corresponding to the all projections
    * that comprise the next coordinate to be appended.
    */
   Projections projections() const
   {
      const auto dim = baseLat().dimension();
      const auto maxOrder = dim;
      const auto maxCoord = dim > 0 ? dim - 1 : 0;
      const auto newCoord = dim;
      return Projections{{0, maxOrder, 0, maxCoord}, newCoord};
   }

   /**
    * Output sequence of merit values.
    *
    * \tparam GENSEQ    Type of sequence of generator values.
    */
   template <typename GENSEQ>
   class Seq :
      public BridgeSeq<
         Seq<GENSEQ>,
         LatSeq::CBC<LAT, GENSEQ>,
         value_type,
         BridgeIteratorCached>
   {
      typedef Seq<GENSEQ> self_type;

   public:

      typedef typename self_type::Base Base;
      typedef typename self_type::value_type value_type;
      typedef typename self_type::size_type size_type;

      /**
       * Constructor.
       *
       * \param parent     Reference to the parent.  Kept as a reference, no
       *                   copy made.
       * \param base       Base CBC lattice sequence.
       */
      Seq(const CBC& parent, Base base):
         self_type::BridgeSeq_(std::move(base)),
         m_parent(parent)
      { }

      /**
       * Computes and returns the value of the figure of merit for the generator
       * value pointed to by \c it.
       */
      MeritValue element(const typename Base::const_iterator& it) const
      {
         return m_parent.evaluator()(
               *it,
               m_parent.projections(),
               m_parent.baseMerit()
               );
      }

   private:
      const CBC& m_parent;
   };

   /**
    * Creates a new sequence of merit values by appending the values in \c
    * genSeq to the generating vector of the base lattice.
    *
    * \param genSeq    Sequence of generator values.
    */
   template <typename GENSEQ>
   Seq<GENSEQ> meritSeq(GENSEQ genSeq) const
   { return Seq<GENSEQ>(*this, LatSeq::cbc(baseLat(), std::move(genSeq))); }

   /**
    * Selects the element pointed to by the iterator.
    *
    * Returns \c true if and only if the sequence of generator sequences is not
    * exhausted.
    *
    * \param it         Iterator on the element of the sequence to be selected.
    */
   template <typename IT>
   void select(const IT& it)
   {
      m_baseMerit = *it;
      m_baseLat = *it.base();
   }

private:
   Storage<LAT, COMPRESS> m_storage;
   const FigureOfMerit& m_figureOfMerit;
   Evaluator m_eval;
   LatDef m_baseLat;
   MeritValue m_baseMerit;
};

/// Creates a CBC algorithm.
template <LatType LAT, Compress COMPRESS, class PROJDEP, template <class> class ACC>
CBC<LAT, COMPRESS, PROJDEP, ACC> cbc(
      Storage<LAT, COMPRESS> storage,
      const WeightedFigureOfMerit<PROJDEP, ACC>& figure)
{ return CBC<LAT, COMPRESS, PROJDEP, ACC>(std::move(storage), figure); }

}}


#endif
