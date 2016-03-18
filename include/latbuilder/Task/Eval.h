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

#ifndef LATBUILDER__TASK__EVAL_H
#define LATBUILDER__TASK__EVAL_H

#include "latbuilder/Task/CBCBasedSearch.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"

#include <vector>

namespace LatBuilder { namespace Task {

template <LatType LAT, Compress COMPRESS, class FIGURE>
struct EvalTag {};


/// Explicit construction (evaluates a figure of merit for a single lattice).
template <LatType LAT, Compress COMPRESS, class FIGURE> using Eval =
   CBCBasedSearch<EvalTag<LAT, COMPRESS, FIGURE>>;


/// Explicit construction (evaluates a figure of merit for a single lattice).
template <class FIGURE, LatType LAT, Compress COMPRESS>
Eval<LAT, COMPRESS, FIGURE> eval(
      Storage<LAT, COMPRESS> storage,
      Dimension dimension,
      FIGURE figure,
      GeneratingVector genVec
      )
{ return Eval<LAT, COMPRESS, FIGURE>(std::move(storage), dimension, std::move(figure), std::move(genVec)); }


template <LatType LAT, Compress COMPRESS, class FIGURE>
struct CBCBasedSearchTraits<EvalTag<LAT, COMPRESS, FIGURE>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef typename CBCSelector<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef std::vector<Modulus> GenSeqType;

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
   { return FIGURE::evaluationName() + " evaluation"; }

   void init(LatBuilder::Task::Eval<LAT, COMPRESS, FIGURE>& search) const
   { connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty()); }

   GeneratingVector genVec;
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, CBCBasedSearch, Eval);

}}

#endif
