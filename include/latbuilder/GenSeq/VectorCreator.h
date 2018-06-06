// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
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

#ifndef LATBUILDER__GEN_SEQ__VECTOR_CREATOR_H
#define LATBUILDER__GEN_SEQ__VECTOR_CREATOR_H

#include "latbuilder/GenSeq/Creator.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/Traversal.h"

#include <vector>

namespace LatBuilder { namespace GenSeq {

namespace detail {
   template <class TRAV>
   class Traversal {
   public:
      template <typename... ARGS>
      Traversal(ARGS&&... args):
         m_trav(std::forward<ARGS>(args)...)
      {}

      TRAV operator()()
      { return m_trav; }

   private:
      TRAV m_trav;
   };

   template <typename RAND>
   class Traversal<LatBuilder::Traversal::Random<RAND>> {
   public:
      Traversal(
            size_t nrand = std::numeric_limits<size_t>::max(),
            RAND rand = RAND()
            ):
         m_nrand(nrand),
         m_rand(std::move(rand))
      {}

      LatBuilder::Traversal::Random<RAND> operator()()
      {
         LatBuilder::Traversal::Random<RAND> trav(m_nrand, m_rand);
         m_rand.jump();
         return trav;
      }

   private:
      size_t m_nrand;
      RAND m_rand;
      LatBuilder::Traversal::Random<RAND> m_trav;
   };
}

/**
 * Creator for vectors of sequences of generator values.
 */
template <typename SEQ>
struct VectorCreator {
   typedef std::vector<SEQ> result_type;

   /**
    * Creates a new sequence object.
    *
    * \param sizeParam        Size parameter for the generator sequences.
    * \param dimension        Dimension of the output vector.
    * \param t                Other arguments to be passed to GenSeq::Creator
    */
   template <LatticeType LR, PointSetType L, typename... ARGS>
   static result_type create(
         const SizeParam<LR,L>& sizeParam,
         Dimension dimension, 
         ARGS&&... t
         )
   {
      result_type vec(dimension);
      detail::Traversal<typename SEQ::Traversal> trav(std::forward<ARGS>(t)...);
      for (Dimension j = 0; j < dimension; j++)
         vec[j] = GenSeq::Creator<SEQ>::create(sizeParam, trav());
      return vec;
   }
};


}}

#endif
