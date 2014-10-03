// Copyright (c) 2012, 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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

#ifndef LATBUILDER__TASK__RANDOM_KOROBOV_H
#define LATBUILDER__TASK__RANDOM_KOROBOV_H

#include "latbuilder/Task/LatSeqBasedSearch.h"
#include "latbuilder/Task/FigureOfMeritTraits.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/LatSeq/Korobov.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/GenSeq/Creator.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR113.h"

#include <boost/lexical_cast.hpp>

namespace LatBuilder { namespace Task {

template <LatType LAT, Compress COMPRESS, class FIGURE>
struct RandomKorobovTag {};


/// Random Korobov search.
template <LatType LAT, Compress COMPRESS, class FIGURE> using RandomKorobov =
   LatSeqBasedSearch<RandomKorobovTag<LAT, COMPRESS, FIGURE>>;


/// Random Korobov search.
template <class FIGURE, LatType LAT, Compress COMPRESS>
RandomKorobov<LAT, COMPRESS, FIGURE> randomKorobov(
      Storage<LAT, COMPRESS> storage,
      Dimension dimension,
      FIGURE figure,
      unsigned int numRand
      )
{ return RandomKorobov<LAT, COMPRESS, FIGURE>(std::move(storage), dimension, std::move(figure), numRand); }


template <LatType LAT, Compress COMPRESS, class FIGURE>
struct LatSeqBasedSearchTraits<RandomKorobovTag<LAT, COMPRESS, FIGURE>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef typename FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef LFSR113 RandomGenerator;
   typedef LatBuilder::Traversal::Random<RandomGenerator> Traversal;
   typedef GenSeq::CoprimeIntegers<COMPRESS, Traversal> GenSeqType;
   typedef LatSeq::Korobov<LAT, GenSeqType> LatSeqType;

   LatSeqBasedSearchTraits(unsigned int numRand_):
      numRand(numRand_)
   {}

   virtual ~LatSeqBasedSearchTraits() {}

   LatSeqType latSeq(const SizeParam& sizeParam, Dimension dimension)
   {
      auto infty = std::numeric_limits<typename Traversal::size_type>::max();
      auto out = LatSeqType(
            sizeParam,
            GenSeq::Creator<GenSeqType>::create(sizeParam, Traversal(infty, rand)),
            dimension
            );
      rand.jump();
      return out;
   }

   std::string name() const
   { return FIGURE::evaluationName() + " random Korobov search (" + boost::lexical_cast<std::string>(numRand) + " random samples)"; }

   void init(LatBuilder::Task::RandomKorobov<LAT, COMPRESS, FIGURE>& search) const
   {
      connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty());
      search.minObserver().setMaxAcceptedCount(numRand);
   }

   unsigned int numRand;
   RandomGenerator rand;
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, LatSeqBasedSearch, RandomKorobov);

}}

#endif
