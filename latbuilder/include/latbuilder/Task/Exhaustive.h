// Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
// 
// This file is part of Lattice Builder.
// 
// Lattice Builder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Lattice Builder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

#ifndef LATBUILDER__TASK__EXHAUSTIVE_H
#define LATBUILDER__TASK__EXHAUSTIVE_H

#include "latbuilder/Task/LatSeqBasedSearch.h"
#include "latbuilder/Task/FigureOfMeritTraits.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/GenSeq/VectorCreator.h"

namespace LatBuilder { namespace Task {

template <LatType LAT, Compress COMPRESS, class FIGURE>
struct ExhaustiveTag {};


/// Exhaustive search.
template <LatType LAT, Compress COMPRESS, class FIGURE> using Exhaustive =
   LatSeqBasedSearch<ExhaustiveTag<LAT, COMPRESS, FIGURE>>;


/// Exhaustive search.
template <class FIGURE, LatType LAT, Compress COMPRESS>
Exhaustive<LAT, COMPRESS, FIGURE> exhaustive(
      Storage<LAT, COMPRESS> storage,
      Dimension dimension,
      FIGURE figure
      )
{ return Exhaustive<LAT, COMPRESS, FIGURE>(std::move(storage), dimension, std::move(figure)); }


template <LatType LAT, Compress COMPRESS, class FIGURE>
struct LatSeqBasedSearchTraits<ExhaustiveTag<LAT, COMPRESS, FIGURE>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef typename FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef GenSeq::CoprimeIntegers<COMPRESS> GenSeqType;
   typedef LatSeq::Combiner<LAT, GenSeqType, CartesianProduct> LatSeqType;

   virtual ~LatSeqBasedSearchTraits() {}

   LatSeqType latSeq(const SizeParam& sizeParam, Dimension dimension) const
   {
      auto vec = GenSeq::VectorCreator<GenSeqType>::create(sizeParam, dimension);
      vec[0] = GenSeq::Creator<GenSeqType>::create(SizeParam(2));
      return LatSeqType(sizeParam, std::move(vec));
   }

   std::string name() const
   { return FIGURE::evaluationName() + " exhaustive search"; }

   void init(LatBuilder::Task::Exhaustive<LAT, COMPRESS, FIGURE>& search) const
   { connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty()); }
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, LatSeqBasedSearch, Exhaustive);

}}

#endif
