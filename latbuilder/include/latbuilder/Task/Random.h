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

#ifndef LATBUILDER__TASK__RANDOM_H
#define LATBUILDER__TASK__RANDOM_H

#include "latbuilder/Task/LatSeqBasedSearch.h"
#include "latbuilder/Task/FigureOfMeritTraits.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR258.h"

#include <boost/lexical_cast.hpp>

namespace LatBuilder { namespace Task {

template <LatType LAT, Compress COMPRESS, class FIGURE>
struct RandomTag {};


/// Fully random search.
template <LatType LAT, Compress COMPRESS, class FIGURE> using Random =
   LatSeqBasedSearch<RandomTag<LAT, COMPRESS, FIGURE>>;


/// Fully random search.
template <class FIGURE, LatType LAT, Compress COMPRESS>
Random<LAT, COMPRESS, FIGURE> random(
      Storage<LAT, COMPRESS> storage,
      Dimension dimension,
      FIGURE figure,
      unsigned int numRand
      )
{ return Random<LAT, COMPRESS, FIGURE>(std::move(storage), dimension, std::move(figure), numRand); }


template <LatType LAT, Compress COMPRESS, class FIGURE>
struct LatSeqBasedSearchTraits<RandomTag<LAT, COMPRESS, FIGURE>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef typename FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef LFSR258 RandomGenerator;
   typedef LatBuilder::Traversal::Random<RandomGenerator> Traversal;
   typedef GenSeq::CoprimeIntegers<COMPRESS, Traversal> GenSeqType;
   typedef LatSeq::Combiner<LAT, GenSeqType, CartesianProduct> LatSeqType;

   LatSeqBasedSearchTraits(unsigned int numRand_):
      numRand(numRand_)
   {}

   LatSeqType latSeq(const SizeParam& sizeParam, Dimension dimension)
   {
      auto infty = std::numeric_limits<typename Traversal::size_type>::max();
      std::vector<GenSeqType> vec;
      vec.reserve(dimension);
      vec.push_back(GenSeq::Creator<GenSeqType>::create(SizeParam(2)));
      vec[0].resize(1);
      for (Dimension j = 1; j < dimension; j++) {
         vec.push_back(
               GenSeq::Creator<GenSeqType>::create(
                  sizeParam,
                  Traversal(infty, rand)
                  )
               );
         rand.jump();
      }
      return LatSeqType(sizeParam, std::move(vec));
   }

   std::string name() const
   { return FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::prefix() + "random search (" + boost::lexical_cast<std::string>(numRand) + " random samples)"; }

   void init(LatBuilder::Task::Random<LAT, COMPRESS, FIGURE>& search) const
   {
      FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::init(search);
      search.minObserver().setMaxAcceptedCount(numRand);
   }

   unsigned int numRand;
   RandomGenerator rand;
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, LatSeqBasedSearch, Random);

}}

#endif
