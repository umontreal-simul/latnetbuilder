// Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
// 
// This file is part of Lattice Builder.
// 
// Lattice Builder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Lattice Builder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

#ifndef LATBUILDER__MERIT_SEQ__COORD_SYM_CBC_H
#define LATBUILDER__MERIT_SEQ__COORD_SYM_CBC_H

#include "latbuilder/CoordSymFigureOfMerit.h"
#include "latbuilder/MeritSeq/CoordSymInnerProd.h"
#include "latbuilder/MeritSeq/CoordSymState.h"
#include "latbuilder/MeritSeq/CoordSymStateCreator.h"

#include "latbuilder/LatDef.h"
#include "latbuilder/Storage.h"

#include "latbuilder/LatSeq/CBC.h"

#include <boost/iterator/iterator_facade.hpp>

#include <functional>


namespace LatBuilder { namespace MeritSeq {

/**
 * Sequence of lattice merit for weighted coordinate-symmetric figures of merit.
 *
 * A weighted coordinate-symmetric figure of merit is of the form:
 * \f[
 *    \mathcal D^2 =
 *    \sum_{\emptyset \neq \mathfrak u \subseteq \{1, \dots, s\}}
 *    \gamma_{\mathfrak u}
 *    \frac1n \sum_{i=0}^{n-1}
 *    \prod_{j \in \mathfrak u} \omega(x_{i,j}),
 * \f]
 * for some function \f$\omega: [0,1) \to [0,\infty)\f$.
 *
 * \tparam LAT          Type of lattice.
 * \tparam COMPRESS     Type of compression.
 * \tparam KERNEL       Kernel of the coordinate-symmetric figure of merit;
 *                      should derive from Kernel::Base.
 * \tparam PROD         Type of inner product; either CoordSymInnerProd
 *                      or CoordSymInnerProdFast.
 */
template <LatType LAT, Compress COMPRESS, class KERNEL, template <LatType, Compress> class PROD = CoordSymInnerProd>
class CoordSymCBC
{
   typedef CoordSymCBC<LAT, COMPRESS, KERNEL, PROD> self_type;

public:
   typedef PROD<LAT, COMPRESS> CoordSymInnerProd;
   typedef typename Storage<LAT, COMPRESS>::MeritValue MeritValue;
   typedef LatBuilder::LatDef<LAT> LatDef;
   typedef CoordSymFigureOfMerit<KERNEL> FigureOfMerit;
   typedef typename CoordSymInnerProd::StateList StateList;
   typedef MeritValue value_type;

   /**
    * Constructor.
    *
    * \param storage       Storage configuration.
    * \param figure        Coordinate-symmetric figure of merit.
    */
   CoordSymCBC(
         Storage<LAT, COMPRESS> storage,
         const FigureOfMerit& figure
         ):
      m_storage(std::move(storage)),
      m_figure(std::move(figure)),
      m_innerProd(this->storage(), this->figureOfMerit().kernel()),
      m_states(CoordSymStateCreator::create(m_innerProd.internalStorage(), this->figureOfMerit().weights())),
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
      for (auto& state : m_states)
         state->reset();
   }

   /**
    * Returns the storage configuration instance.
    */
   const Storage<LAT, COMPRESS>& storage() const
   { return m_storage; }

   /**
    * Returns the coordinate-symmetric figure of merit.
    */
   const FigureOfMerit& figureOfMerit() const
   { return m_figure; }

   /**
    * Returns the inner product instance.
    */
   const CoordSymInnerProd& innerProd() const
   { return m_innerProd; }

   /**
    * Returns the states.
    */
   const StateList& states() const
   { return m_states; }

   /**
    * Returns the total weighted state.
    */
   RealVector weightedState() const
   {
      auto it = states().begin();
      if (it == states().end())
         throw std::runtime_error("CoordSymCBC: empty list of states");
      auto out = (*it)->weightedState();
      while (++it != states().end())
         out += (*it)->weightedState();
      return out;
   }

   //! \copydoc CBC::baseLat()
   const LatDef& baseLat() const
   { return m_baseLat; }

   //! \copydoc CBC::baseMerit()
   const MeritValue& baseMerit() const
   { return m_baseMerit; }

   /**
    * Output sequence from the coordinate-symmetric CBC algorithm.
    * It is based on a sequence of lattice definitions.
    *
    * \tparam GENSEQ    Type of sequence of generator values.
    */
   template <typename GENSEQ>
   class Seq {
   public:
      typedef GENSEQ GenSeq;
      typedef typename CoordSymInnerProd::template Seq<GenSeq> ProdSeq;
      typedef LatBuilder::LatSeq::CBC<LAT, GenSeq> LatSeq;
      typedef typename LatSeq::size_type size_type;

      /**
       * Constructor.
       *
       * \param parent     Reference to the parent.  Kept as a reference, no
       *                   copy made.
       * \param genSeq     Sequence of generator values.
       */
      Seq(const CoordSymCBC& parent, const GenSeq& genSeq):
         m_parent(parent),
         m_latSeq(m_parent.baseLat(), genSeq),
         m_prodSeq(m_parent.innerProd().prodSeq(
                  genSeq,
                  parent.weightedState()
                  ))
      {}

      /**
       * Returns a reference to the parent CoordSymCBC instance.
       */
      const CoordSymCBC& cbc() const
      { return m_parent; }

      /**
       * Returns a reference to the internal lattice sequence.
       */
      const LatSeq& latSeq() const
      { return m_latSeq; }

      /**
       * Returns a reference to the internal inner product sequence.
       */
      const ProdSeq& prodSeq() const
      { return m_prodSeq; }

      /**
       * Constant iterator.
       */
      class const_iterator :
         public boost::iterator_facade<
            const_iterator,
            const MeritValue&,
            boost::forward_traversal_tag>
      {
      public:

         struct end {};

         explicit const_iterator(const Seq& seq):
            m_seq(&seq),
            m_lat(m_seq->latSeq().begin()),
            m_prod(m_seq->prodSeq().begin()),
            m_cached(false)
         {}

         const_iterator(const Seq& seq, const end&):
            m_seq(&seq),
            m_lat(m_seq->latSeq().end()),
            m_prod(m_seq->prodSeq().end()),
            m_cached(false)
         {}

         /**
          * Returns a reference to the sequence.
          */
         const Seq& seq() const
         { return *m_seq; }

         /**
          * Returns a reference to the internal lattice sequence iterator.
          */
         const typename LatSeq::const_iterator& base() const
         { return m_lat; }

      private:
         friend class boost::iterator_core_access;

         bool equal(const const_iterator& other) const
         { return m_seq == other.m_seq and m_lat == other.m_lat; }

         const MeritValue& dereference() const
         {
            if (not m_cached) {
               m_value = computeValue();
               m_cached = true;
            }
            return m_value;
         }

         void increment()
         {
            ++m_prod;
            ++m_lat;
            m_cached = false;
         }

         MeritValue computeValue() const
         {
            auto merit = *m_prod;
            m_lat->sizeParam().normalize(merit);
            merit += seq().cbc().baseMerit();
            return merit;
         }

      private:
         const Seq* m_seq;
         typename LatSeq::const_iterator m_lat;
         typename ProdSeq::const_iterator m_prod;
         mutable bool m_cached;
         mutable MeritValue m_value;
      };

      /**
       * Returns an iterator pointing to the first element in the sequence.
       */
      const_iterator begin() const
      { return const_iterator(*this); }

      /**
       * Returns an iterator pointing past the last element in the sequence.
       */
      const_iterator end() const
      { return const_iterator(*this, typename const_iterator::end()); }

   private:
      const CoordSymCBC& m_parent;
      const LatSeq m_latSeq;
      const ProdSeq m_prodSeq;
   };

   /**
    * Creates a new sequence of merit values by appending the values in \c
    * genSeq to the generating vector of the base lattice.
    *
    * \param genSeq    Sequence of generator values.
    */
   template <typename GENSEQ>
   Seq<GENSEQ> meritSeq(const GENSEQ& genSeq) const
   { return Seq<GENSEQ>(*this, genSeq); }

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
      Modulus gen = *it.base().base();
      for (auto& state : m_states)
         state->update(m_innerProd.kernelValues(), gen);
   }

private:
   Storage<LAT, COMPRESS> m_storage;
   const FigureOfMerit& m_figure;
   CoordSymInnerProd m_innerProd;
   StateList m_states;

   LatDef m_baseLat;
   MeritValue m_baseMerit;
};


/// Creates a coordinate-symmetric CBC algorithm.
template <template <LatType, Compress> class PROD = CoordSymInnerProd, LatType LAT, Compress COMPRESS, class KERNEL>
CoordSymCBC<LAT, COMPRESS, KERNEL, PROD> cbc(
      Storage<LAT, COMPRESS> storage,
      const CoordSymFigureOfMerit<KERNEL>& figure
      )
{ return CoordSymCBC<LAT, COMPRESS, KERNEL, PROD>(std::move(storage), figure); }

}}

#endif
