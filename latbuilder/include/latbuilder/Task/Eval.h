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

#ifndef LATBUILDER__TASK__EVAL_H
#define LATBUILDER__TASK__EVAL_H

#include "latbuilder/Task/CBCBasedSearch.h"
#include "latbuilder/Task/FigureOfMeritTraits.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/CoordSymFigureOfMerit.h"
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
   typedef typename FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef std::vector<Modulus> GenSeqType;

   CBCBasedSearchTraits(GeneratingVector genVec): genVec(std::move(genVec)) {}

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
   { return FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::prefix() + "evaluation"; }

   void init(LatBuilder::Task::Eval<LAT, COMPRESS, FIGURE>& search) const
   { FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::init(search); }

   GeneratingVector genVec;
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, CBCBasedSearch, Eval);

}}

#endif
