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
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/GenSeq/Creator.h"
#include "latbuilder/SizeParam.h"

namespace LatBuilder { namespace Task {

template <LatType LAT, Compress COMPRESS, class FIGURE>
struct KorobovTag {};


/// Korobov search.
template <LatType LAT, Compress COMPRESS, class FIGURE> using Korobov =
   LatSeqBasedSearch<KorobovTag<LAT, COMPRESS, FIGURE>>;


/// Korobov search.
template <class FIGURE, LatType LAT, Compress COMPRESS>
Korobov<LAT, COMPRESS, FIGURE> korobov(
      Storage<LAT, COMPRESS> storage,
      Dimension dimension,
      FIGURE figure
      )
{ return Korobov<LAT, COMPRESS, FIGURE>(std::move(storage), dimension, std::move(figure)); }


template <LatType LAT, Compress COMPRESS, class FIGURE>
struct LatSeqBasedSearchTraits<KorobovTag<LAT, COMPRESS, FIGURE>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef typename CBCSelector<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef GenSeq::CoprimeIntegers<COMPRESS> GenSeqType;
   typedef LatSeq::Korobov<LAT, GenSeqType> LatSeqType;

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

   void init(LatBuilder::Task::Korobov<LAT, COMPRESS, FIGURE>& search) const
   { connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty()); }
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, LatSeqBasedSearch, Korobov);

}}

#endif
