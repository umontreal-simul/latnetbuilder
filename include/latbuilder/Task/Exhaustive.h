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

#ifndef LATBUILDER__TASK__EXHAUSTIVE_H
#define LATBUILDER__TASK__EXHAUSTIVE_H

#include "latbuilder/Task/LatSeqBasedSearch.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/Util.h"

namespace LatBuilder { namespace Task {

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct ExhaustiveTag {};


/// Exhaustive search.
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class FIGURE> using Exhaustive =
   LatSeqBasedSearch<ExhaustiveTag<LR, ET, COMPRESS, PLO, FIGURE>>;


/// Exhaustive search.
template <class FIGURE, LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
Exhaustive<LR, ET, COMPRESS, PLO, FIGURE> exhaustive(
      Storage<LR, ET, COMPRESS, PLO> storage,
      Dimension dimension,
      FIGURE figure
      )
{ return Exhaustive<LR, ET, COMPRESS, PLO, FIGURE>(std::move(storage), dimension, std::move(figure)); }


template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct LatSeqBasedSearchTraits<ExhaustiveTag<LR, ET, COMPRESS, PLO, FIGURE>> {
   typedef LatBuilder::Task::Search<LR, ET> Search;
   typedef LatBuilder::Storage<LR, ET, COMPRESS, PLO> Storage;
   typedef typename LatBuilder::Storage<LR, ET, COMPRESS, PLO>::SizeParam SizeParam;
   typedef typename CBCSelector<LR, ET, COMPRESS, PLO, FIGURE>::CBC CBC;
   typedef GenSeq::GeneratingValues<LR, COMPRESS> GenSeqType;
   typedef LatSeq::Combiner<LR, ET, GenSeqType, CartesianProduct> LatSeqType;

   virtual ~LatSeqBasedSearchTraits() {}

   LatSeqType latSeq(const SizeParam& sizeParam, Dimension dimension) const
   {
      auto vec = GenSeq::VectorCreator<GenSeqType>::create(sizeParam, dimension);
      vec[0] = GenSeq::Creator<GenSeqType>::create(SizeParam(LatticeTraits<LR>::TrivialModulus));
      return LatSeqType(sizeParam, std::move(vec));
   }

   std::string name() const
   { return "Task: LatBuilder Search for " + to_string(LR)  + " lattices\nExploration method: Exhaustive";}

   void init(LatBuilder::Task::Exhaustive<LR, ET, COMPRESS, PLO, FIGURE>& search) const
   { connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty()); }
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, LatSeqBasedSearch, Exhaustive);

}}

#endif
