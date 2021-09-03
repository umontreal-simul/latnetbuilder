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

#ifndef LATBUILDER__GEN_SEQ__CREATOR_H
#define LATBUILDER__GEN_SEQ__CREATOR_H

#include "latbuilder/GenSeq/CyclicGroup.h"
#include "latbuilder/GenSeq/PowerSeq.h"
#include "latbuilder/SizeParam.h"

namespace LatBuilder { namespace GenSeq {

/**
 * Creator for sequences of generator values.
 *
 * It is an adaptor for the different constructor signatures of the generator
 * sequences.
 * It allows to use the same constructor arguments regardless of the sequence
 * type.
 *
 */
template <typename SEQ>
struct Creator {
   typedef SEQ result_type;

   /**
    * Creates a new sequence object.
    */
   template <LatticeType LR, EmbeddingType L, typename... ARGS>
   static result_type create(
         const SizeParam<LR, L>& sizeParam,
         ARGS&&... t
         )
   { return result_type(sizeParam.modulus(), std::forward<ARGS>(t)...); }
};

/**
 * Creator specialization for cyclic groups.
 */
template <LatticeType LR,Compress COMPRESS, class TRAV, GroupOrder ORDER>
struct Creator<CyclicGroup<LR,COMPRESS, TRAV, ORDER>> {
   typedef CyclicGroup<LR, COMPRESS, TRAV, ORDER> result_type;

   template <typename... ARGS>
   static result_type create(
         const SizeParam<LR,EmbeddingType::MULTILEVEL>& sizeParam,
         ARGS&&... t
         )
   { return result_type(sizeParam.base(), sizeParam.maxLevel(), std::forward<ARGS>(t)...); }

   template <typename... ARGS>
   static result_type create(
         const SizeParam<LR,EmbeddingType::UNILEVEL>& sizeParam,
         ARGS&&... t
         )
   {
      SizeParam<LR, EmbeddingType::MULTILEVEL> ml(sizeParam.modulus());
      return result_type(ml.base(), ml.maxLevel(), std::forward<ARGS>(t)...);
   }
};

/**
 * Creator specialization for PowerSeq.
 */
template <typename SEQ>
struct Creator<PowerSeq<SEQ>> {
   typedef PowerSeq<SEQ> result_type;
   /**
    * Creates a new sequence object.
    */
   template <LatticeType LR, EmbeddingType L, typename... ARGS>
   static result_type create(
         const SizeParam<LR, L>& sizeParam,
         unsigned int power,
         ARGS&&... t
         )
   { return result_type(Creator<SEQ>::create(sizeParam, std::forward<ARGS>(t)...), power, sizeParam.modulus()); }
};

}}

#endif
