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

#ifndef LATBUILDER__TASK__CBC_H
#define LATBUILDER__TASK__CBC_H

#include "latbuilder/Task/CBCBasedSearch.h"
#include "latbuilder/Task/FigureOfMeritTraits.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/CoordSymFigureOfMerit.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/GenSeq/VectorCreator.h"

namespace LatBuilder { namespace Task {

template <LatType LAT, Compress COMPRESS, class FIGURE>
struct CBCTag {};


/// CBC construction.
template <LatType LAT, Compress COMPRESS, class FIGURE> using CBC =
   CBCBasedSearch<CBCTag<LAT, COMPRESS, FIGURE>>;


/// CBC construction.
template <class FIGURE, LatType LAT, Compress COMPRESS>
CBC<LAT, COMPRESS, FIGURE> cbc(
      Storage<LAT, COMPRESS> storage,
      Dimension dimension,
      FIGURE figure
      )
{ return CBC<LAT, COMPRESS, FIGURE>(std::move(storage), dimension, std::move(figure)); }


template <LatType LAT, Compress COMPRESS, class FIGURE>
struct CBCBasedSearchTraits<CBCTag<LAT, COMPRESS, FIGURE>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef typename FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef GenSeq::CoprimeIntegers<COMPRESS> GenSeqType;

   std::vector<GenSeqType> genSeqs(const SizeParam& sizeParam, Dimension dimension) const
   {
      auto vec = GenSeq::VectorCreator<GenSeqType>::create(sizeParam, dimension);
      vec[0] = GenSeq::Creator<GenSeqType>::create(SizeParam(2));
      return vec;
   }

   std::string name() const
   { return FIGURE::evaluationName() + " CBC"; }

   void init(LatBuilder::Task::CBC<LAT, COMPRESS, FIGURE>& search) const
   { FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::init(search); }
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, CBCBasedSearch, CBC);

}}

#endif
