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

#ifndef LATBUILDER__TASK__KOROBOV_H
#define LATBUILDER__TASK__KOROBOV_H

#include "latbuilder/Task/LatSeqBasedSearch.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/LatSeq/Korobov.h"
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/GenSeq/Creator.h"
#include "latbuilder/SizeParam.h"

namespace LatBuilder { namespace Task {

template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO, class FIGURE>
struct KorobovTag {};


/// Korobov search.
template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO, class FIGURE> using Korobov =
   LatSeqBasedSearch<KorobovTag<LR, LAT, COMPRESS, PLO, FIGURE>>;


/// Korobov search.
template <class FIGURE, Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
Korobov<LR, LAT, COMPRESS, PLO, FIGURE> korobov(
      Storage<LR, LAT, COMPRESS, PLO> storage,
      Dimension dimension,
      FIGURE figure
      )
{ return Korobov<LR, LAT, COMPRESS, PLO, FIGURE>(std::move(storage), dimension, std::move(figure)); }


template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO, class FIGURE>
struct LatSeqBasedSearchTraits<KorobovTag<LR, LAT, COMPRESS, PLO, FIGURE>> {
   typedef LatBuilder::Task::Search<LR, LAT> Search;
   typedef LatBuilder::Storage<LR, LAT, COMPRESS, PLO> Storage;
   typedef typename LatBuilder::Storage<LR, LAT, COMPRESS, PLO>::SizeParam SizeParam;
   typedef typename CBCSelector<LR, LAT, COMPRESS, PLO, FIGURE>::CBC CBC;
   typedef GenSeq::GeneratingValues<LR, COMPRESS> GenSeqType;
   typedef LatSeq::Korobov<LR, LAT, GenSeqType> LatSeqType;

   virtual ~LatSeqBasedSearchTraits() {}

   LatSeqType latSeq(const SizeParam& sizeParam, Dimension dimension) const
   {
      return LatSeqType(
            sizeParam,
            GenSeq::Creator<GenSeqType>::create(sizeParam),
            dimension
            );
   }

   std::string name() const
   { return FIGURE::evaluationName() + " Korobov search"; }

   void init(LatBuilder::Task::Korobov<LR, LAT, COMPRESS, PLO, FIGURE>& search) const
   { connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty()); }
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, LatSeqBasedSearch, Korobov);

}}

#endif
