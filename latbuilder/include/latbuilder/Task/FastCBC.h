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

#ifndef LATBUILDER__TASK__FAST_CBC_H
#define LATBUILDER__TASK__FAST_CBC_H

#include "latbuilder/Task/CBCBasedSearch.h"
#include "latbuilder/Task/FigureOfMeritTraits.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/CoordSymFigureOfMerit.h"
#include "latbuilder/MeritSeq/CoordSymInnerProdFast.h"
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

// specialization for coordinate-symmetric figures of merit
template <LatType LAT, Compress COMPRESS, class KERNEL>
struct CBCBasedSearchTraits<FastCBCTag<LAT, COMPRESS, CoordSymFigureOfMerit<KERNEL>>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef MeritSeq::CoordSymCBC<LAT, COMPRESS, KERNEL, MeritSeq::CoordSymInnerProdFast> CBC;
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
   { FigureOfMeritTraits<LAT, COMPRESS, FigureOfMerit>::init(search); }
};

// specialization for other figures of merit
template <LatType LAT, Compress COMPRESS, class FIGURE>
struct CBCBasedSearchTraits<FastCBCTag<LAT, COMPRESS, FIGURE>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef FIGURE FigureOfMerit;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef typename FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::CBC CBC;
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
   { throw std::runtime_error("fast CBC not implemented for this type of figure of merit"); }
};

TASK_FOR_ALL_COORDSYM(TASK_EXTERN_TEMPLATE, CBCBasedSearch, FastCBC);

}}

#endif
