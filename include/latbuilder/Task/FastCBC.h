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

#ifndef LATBUILDER__TASK__FAST_CBC_H
#define LATBUILDER__TASK__FAST_CBC_H

#include "latbuilder/Task/CBCBasedSearch.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latbuilder/MeritSeq/CoordUniformInnerProdFast.h"
#include "latbuilder/GenSeq/CyclicGroup.h"
#include "latbuilder/GenSeq/VectorCreator.h"

namespace LatBuilder { namespace Task {

template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct FastCBCTag {};


/// Fast CBC construction.
template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class FIGURE> using FastCBC =
   CBCBasedSearch<FastCBCTag<LR, PST, COMPRESS, PLO, FIGURE>>;


/// Fast CBC construction.
template <class FIGURE,LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO>
FastCBC<LR, PST, COMPRESS, PLO, FIGURE> fastCBC(
      Storage<LR, PST, COMPRESS, PLO> storage,
      Dimension dimension,
      FIGURE figure
      )
{ return FastCBC<LR, PST, COMPRESS, PLO, FIGURE>(std::move(storage), dimension, std::move(figure)); }

// specialization for coordinate-uniform figures of merit
template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class KERNEL>
struct CBCBasedSearchTraits<FastCBCTag<LR, PST, COMPRESS, PLO, CoordUniformFigureOfMerit<KERNEL>>> {
   typedef LatBuilder::Task::Search<LR, PST> Search;
   typedef LatBuilder::Storage<LR, PST, COMPRESS, PLO> Storage;
   typedef typename LatBuilder::Storage<LR, PST, COMPRESS, PLO>::SizeParam SizeParam;
   typedef MeritSeq::CoordUniformCBC<LR, PST, COMPRESS, PLO, KERNEL, MeritSeq::CoordUniformInnerProdFast> CBC;
   typedef typename CBC::FigureOfMerit FigureOfMerit;
   typedef GenSeq::CyclicGroup<LR, COMPRESS> GenSeqType;

   std::vector<GenSeqType> genSeqs(const SizeParam& sizeParam, Dimension dimension) const
   {
      auto vec = GenSeq::VectorCreator<GenSeqType>::create(sizeParam, dimension);
      vec[0] = GenSeq::Creator<GenSeqType>::create(SizeParam(LatticeTraits<LR>::TrivialModulus));
      return vec;
   }

   std::string name() const
   { return "fast CBC"; }

   void init(LatBuilder::Task::FastCBC<LR, PST, COMPRESS, PLO, FigureOfMerit>& search) const
   { connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty()); }
};

// specialization for other figures of merit
template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct CBCBasedSearchTraits<FastCBCTag<LR, PST, COMPRESS, PLO, FIGURE>> {
   typedef LatBuilder::Task::Search<LR, PST> Search;
   typedef LatBuilder::Storage<LR, PST, COMPRESS, PLO> Storage;
   typedef FIGURE FigureOfMerit;
   typedef typename LatBuilder::Storage<LR, PST, COMPRESS, PLO>::SizeParam SizeParam;
   typedef typename CBCSelector<LR, PST, COMPRESS, PLO, FIGURE>::CBC CBC;
   typedef GenSeq::CyclicGroup<LR, COMPRESS> GenSeqType;

   virtual ~CBCBasedSearchTraits() {}

   std::vector<GenSeqType> genSeqs(const SizeParam& sizeParam, Dimension dimension) const
   {
      auto vec = GenSeq::VectorCreator<GenSeqType>::create(sizeParam, dimension);
      vec[0] = GenSeq::Creator<GenSeqType>::create(SizeParam(LatticeTraits<LR>::TrivialModulus));
      return vec;
   }

   std::string name() const
   { return "unimplemented fast CBC"; }

   void init(LatBuilder::Task::FastCBC<LR, PST, COMPRESS, PLO, FIGURE>& search) const
   { throw std::runtime_error("fast CBC is implemented only for coordinate-uniform figures of merit"); }
};

TASK_FOR_ALL_COORDSYM(TASK_EXTERN_TEMPLATE, CBCBasedSearch, FastCBC);

}}

#endif
