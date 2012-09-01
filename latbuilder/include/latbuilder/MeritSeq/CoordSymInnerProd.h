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

#ifndef LATBUILDER__MERIT_SEQ__INNER_PROD_H
#define LATBUILDER__MERIT_SEQ__INNER_PROD_H

#include "latbuilder/MeritSeq/CoordSymStateCreator.h"
#include "latbuilder/BridgeSeq.h"
#include "latbuilder/BridgeIteratorCached.h"
#include "latbuilder/Kernel/Base.h"
#include "latbuilder/Storage.h"
#include "latbuilder/CompressedSum.h"

#include <boost/numeric/ublas/expression_types.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

namespace LatBuilder { namespace MeritSeq {

/**
 * Standard implementation of the inner product for a sequence of vector with a
 * single vector.
 */
template <LatType LAT, Compress COMPRESS>
class CoordSymInnerProd {
public:
   typedef Storage<LAT, COMPRESS> InternalStorage;
   typedef CoordSymStateList<LAT, COMPRESS> StateList;
   typedef typename Storage<LAT, COMPRESS>::MeritValue MeritValue;

   /**
    * Constructor.
    *
    * \param storage       Storage configuration.
    * \param kernel        Kernel.  Used to create a sequence of
    *                      permuatations of the kernel values evaluated at every
    *                      one-dimensional lattice point.
    */
   template <class K>
   CoordSymInnerProd(
         Storage<LAT, COMPRESS> storage,
         const Kernel::Base<K>& kernel
         ):
      m_storage(std::move(storage)),
      m_kernelValues(kernel.valuesVector(this->internalStorage()))
   {}

   /**
    * Returns the storage configuration instance.
    */
   const Storage<LAT, COMPRESS>& storage() const
   { return m_storage; }

   /**
    * Returns the storage configuration instance.
    */
   const Storage<LAT, COMPRESS>& internalStorage() const
   { return m_storage; }

   /**
    * Returns the vector of kernel values.
    */
   const RealVector& kernelValues() const
   { return m_kernelValues; }


public:
   /**
    * Sequence of inner product values.
    *
    * \tparam GENSEQ    Type of sequence of generator values.
    */
   template <class GENSEQ>
   class Seq :
      public BridgeSeq<
         Seq<GENSEQ>,                           // self type
         GENSEQ,                                // base type
         MeritValue,                            // value type
         BridgeIteratorCached> {

   public:

      typedef GENSEQ GenSeq;
      typedef typename Seq::Base Base;
      typedef typename Seq::size_type size_type;

      /**
       * Constructor.
       *
       * \param parent     Parent inner product instance.
       * \param genSeq     Sequence of generator sequences that determines the
       *                   order of the permutations of \c baseVec.
       * \param vec        Second operand in the inner product.
       */
      template <typename E>
      Seq(
            const CoordSymInnerProd& parent, 
            GenSeq genSeq,
            const boost::numeric::ublas::vector_expression<E>& vec
         ):
         Seq::BridgeSeq_(std::move(genSeq)),
         m_parent(parent),
         m_constVec(vec())
      { 
         const auto size = m_parent.internalStorage().size();
         if (m_constVec.size() != size)
            throw std::logic_error("invalid size of weighted state vector");
      }

      MeritValue element(const typename Base::const_iterator& it) const
      {
         const auto& st = m_parent.internalStorage();
         return compressedSum(
               st,
               boost::numeric::ublas::element_prod(
                  m_constVec,
                  st.strided(m_parent.m_kernelValues, *it)
                  )
               );
      }

      /**
       * Returns the parent inner product of this sequence.
       */
      const CoordSymInnerProd& innerProd() const
      { return m_parent; }

   private:
      const CoordSymInnerProd& m_parent;
      const RealVector m_constVec;
   };

   /**
    * Creates a new sequence of inner product values by applying a stride
    * permutation based on \c genSeq to the vector of kernel values, then by
    * computing the inner product with \c vec.
    *
    * \param genSeq     Sequence of generator values.
    * \param vec        Second operand in the inner product.
    */
   template <typename GENSEQ, typename E>
   Seq<GENSEQ> prodSeq(
         const GENSEQ& genSeq,
         const boost::numeric::ublas::vector_expression<E>& vec
         ) const
   { return Seq<GENSEQ>(*this, genSeq, vec); }


private:
   template <class> friend class Seq;

private:
   Storage<LAT, COMPRESS> m_storage;
   RealVector m_kernelValues;
};

}}

#endif
