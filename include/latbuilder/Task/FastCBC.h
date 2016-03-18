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

#ifndef LATBUILDER__TASK__FAST_CBC_H
#define LATBUILDER__TASK__FAST_CBC_H

#include "latbuilder/Task/CBCBasedSearch.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latbuilder/MeritSeq/CoordUniformInnerProdFast.h"
#include "latbuilder/GenSeq/CyclicGroup.h"
#include "latbuilder/GenSeq/VectorCreator.h"

namespace LatBuilder { namespace Task {

template <LatType LAT, Compress COMPRESS, class FIGURE>
struct FastCBCTag {};


/// Fast CBC construction.
template <LatType LAT, Compress COMPRESS, class FIGURE> using FastCBC =
   CBCBasedSearch<FastCBCTag<LAT, COMPRESS, FIGURE>>;


/// Fast CBC construction.
template <class FIGURE, LatType LAT, Compress COMPRESS>
FastCBC<LAT, COMPRESS, FIGURE> fastCBC(
      Storage<LAT, COMPRESS> storage,
      Dimension dimension,
      FIGURE figure
      )
{ return FastCBC<LAT, COMPRESS, FIGURE>(std::move(storage), dimension, std::move(figure)); }

// specialization for coordinate-uniform figures of merit
template <LatType LAT, Compress COMPRESS, class KERNEL>
struct CBCBasedSearchTraits<FastCBCTag<LAT, COMPRESS, CoordUniformFigureOfMerit<KERNEL>>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef MeritSeq::CoordUniformCBC<LAT, COMPRESS, KERNEL, MeritSeq::CoordUniformInnerProdFast> CBC;
   typedef typename CBC::FigureOfMerit FigureOfMerit;
   typedef GenSeq::CyclicGroup<COMPRESS> GenSeqType;

   std::vector<GenSeqType> genSeqs(const SizeParam& sizeParam, Dimension dimension) const
   {
      auto vec = GenSeq::VectorCreator<GenSeqType>::create(sizeParam, dimension);
      vec[0] = GenSeq::Creator<GenSeqType>::create(SizeParam(2));
      return vec;
   }

   std::string name() const
   { return "fast CBC"; }

   void init(LatBuilder::Task::FastCBC<LAT, COMPRESS, FigureOfMerit>& search) const
   { connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty()); }
};

// specialization for other figures of merit
template <LatType LAT, Compress COMPRESS, class FIGURE>
struct CBCBasedSearchTraits<FastCBCTag<LAT, COMPRESS, FIGURE>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef FIGURE FigureOfMerit;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef typename CBCSelector<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef GenSeq::CyclicGroup<COMPRESS> GenSeqType;

   virtual ~CBCBasedSearchTraits() {}

   std::vector<GenSeqType> genSeqs(const SizeParam& sizeParam, Dimension dimension) const
   {
      auto vec = GenSeq::VectorCreator<GenSeqType>::create(sizeParam, dimension);
      vec[0] = GenSeq::Creator<GenSeqType>::create(SizeParam(2));
      return vec;
   }

   std::string name() const
   { return "unimplemented fast CBC"; }

   void init(LatBuilder::Task::FastCBC<LAT, COMPRESS, FIGURE>& search) const
   { throw std::runtime_error("fast CBC is implemented only for coordinate-uniform figures of merit"); }
};

TASK_FOR_ALL_COORDSYM(TASK_EXTERN_TEMPLATE, CBCBasedSearch, FastCBC);

}}

#endif
