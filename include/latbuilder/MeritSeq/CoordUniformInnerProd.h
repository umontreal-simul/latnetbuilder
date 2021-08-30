// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#ifndef LATBUILDER__MERIT_SEQ__INNER_PROD_H
#define LATBUILDER__MERIT_SEQ__INNER_PROD_H

#include "latbuilder/MeritSeq/CoordUniformStateCreator.h"
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
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO >
class CoordUniformInnerProd {
public:
   typedef Storage<LR, ET, COMPRESS, PLO> InternalStorage;
   typedef CoordUniformStateList<LR, ET, COMPRESS, PLO> StateList;
   typedef typename Storage<LR, ET, COMPRESS, PLO>::MeritValue MeritValue;

   /**
    * Constructor.
    *
    * \param storage       Storage configuration.
    * \param kernel        Kernel.  Used to create a sequence of
    *                      permuatations of the kernel values evaluated at every
    *                      one-dimensional lattice point.
    */
   template <class K>
   CoordUniformInnerProd(
         Storage<LR, ET, COMPRESS, PLO> storage,
         const Kernel::Base<K>& kernel
         ):
      m_storage(std::move(storage)),
      m_kernelValues(kernel.valuesVector(this->internalStorage()))
   {}

   /**
    * Returns the storage configuration instance.
    */
   const Storage<LR, ET, COMPRESS, PLO>& storage() const
   { return m_storage; }

   /**
    * Returns the storage configuration instance.
    */
   const Storage<LR, ET, COMPRESS, PLO>& internalStorage() const
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
            const CoordUniformInnerProd& parent, 
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
      const CoordUniformInnerProd& innerProd() const
      { return m_parent; }

   private:
      const CoordUniformInnerProd& m_parent;
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
   Storage<LR, ET, COMPRESS, PLO> m_storage;
   RealVector m_kernelValues;
};

}}

#endif
