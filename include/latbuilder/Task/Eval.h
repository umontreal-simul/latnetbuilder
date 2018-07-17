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

#ifndef LATBUILDER__TASK__EVAL_H
#define LATBUILDER__TASK__EVAL_H

#include "latbuilder/Task/CBCBasedSearch.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/GenSeq/GeneratingValues.h"

#include <vector>

namespace LatBuilder { namespace Task {

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct EvalTag {};


/// Explicit construction (evaluates a figure of merit for a single lattice).
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class FIGURE> using Eval =
   CBCBasedSearch<EvalTag<LR, ET, COMPRESS, PLO, FIGURE>>;


/// Explicit construction (evaluates a figure of merit for a single lattice).
template <class FIGURE, LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
Eval<LR, ET, COMPRESS, PLO, FIGURE> eval(
      Storage<LR, ET, COMPRESS, PLO> storage,
      Dimension dimension,
      FIGURE figure,
      typename LatticeTraits<LR>::GeneratingVector genVec
      )
{ return Eval<LR, ET, COMPRESS, PLO, FIGURE>(std::move(storage), dimension, std::move(figure), std::move(genVec)); }


template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct CBCBasedSearchTraits<EvalTag<LR, ET, COMPRESS, PLO, FIGURE>> {
   typedef LatBuilder::Task::Search<LR, ET> Search;
   typedef LatBuilder::Storage<LR, ET, COMPRESS, PLO> Storage;
   typedef typename LatBuilder::Storage<LR, ET, COMPRESS, PLO>::SizeParam SizeParam;
   typedef typename CBCSelector<LR, ET, COMPRESS, PLO, FIGURE>::CBC CBC;
   typedef typename LatticeTraits<LR>::GeneratingVector GeneratingVector;
   typedef std::vector< typename LatticeTraits<LR>::GenValue> GenSeqType;

   CBCBasedSearchTraits(GeneratingVector genVec): genVec(std::move(genVec)) {}

   virtual ~CBCBasedSearchTraits() {}

   std::vector<GenSeqType> genSeqs(const SizeParam& sizeParam, Dimension dimension) const
   {
      if (dimension > genVec.size())
         throw std::runtime_error("dimension > generating vector size");
      std::vector<GenSeqType> out(dimension);
      for (size_t j = 0; j < dimension; j++)
         out[j] = GenSeqType{genVec[j]};
      return out;
   }

   std::string name() const
   {  using TextStream::operator<<;
      std::ostringstream stream;
      stream << "Task: LatBuilder Evaluation of the " << LatticeTypeStrings[(int) LR] << " lattice " << genVec ;
      return stream.str(); }

   void init(LatBuilder::Task::Eval<LR, ET, COMPRESS, PLO, FIGURE>& search) const
   { connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty()); }

   GeneratingVector genVec;
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, CBCBasedSearch, Eval);

}}

#endif
