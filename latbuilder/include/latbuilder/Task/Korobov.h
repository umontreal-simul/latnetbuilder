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

#ifndef LATBUILDER__TASK__KOROBOV_H
#define LATBUILDER__TASK__KOROBOV_H

#include "latbuilder/Task/LatSeqBasedSearch.h"
#include "latbuilder/Task/FigureOfMeritTraits.h"
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
   typedef typename FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef GenSeq::CoprimeIntegers<COMPRESS> GenSeqType;
   typedef LatSeq::Korobov<LAT, GenSeqType> LatSeqType;

   LatSeqType latSeq(const SizeParam& sizeParam, Dimension dimension) const
   {
      return LatSeqType(
            sizeParam,
            GenSeq::Creator<GenSeqType>::create(sizeParam),
            dimension
            );
   }

   std::string name() const
   { return FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::prefix() + "Korobov search"; }

   void init(LatBuilder::Task::Korobov<LAT, COMPRESS, FIGURE>& search) const
   { FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::init(search); }
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, LatSeqBasedSearch, Korobov);

}}

#endif
