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

#ifndef LATBUILDER__TASK__RANDOM_CBC_H
#define LATBUILDER__TASK__RANDOM_CBC_H

#include "latbuilder/Task/CBCBasedSearch.h"
#include "latbuilder/Task/FigureOfMeritTraits.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR113.h"

#include <boost/lexical_cast.hpp>

namespace LatBuilder { namespace Task {

template <LatType LAT, Compress COMPRESS, class FIGURE>
struct RandomCBCTag {};


/// Random CBC construction.
template <LatType LAT, Compress COMPRESS, class FIGURE> using RandomCBC =
   CBCBasedSearch<RandomCBCTag<LAT, COMPRESS, FIGURE>>;


/// Random CBC construction.
template <class FIGURE, LatType LAT, Compress COMPRESS>
RandomCBC<LAT, COMPRESS, FIGURE> randomCBC(
      Storage<LAT, COMPRESS> storage,
      Dimension dimension,
      FIGURE figure,
      unsigned int numRand
      )
{ return RandomCBC<LAT, COMPRESS, FIGURE>(std::move(storage), dimension, std::move(figure), numRand); }


template <LatType LAT, Compress COMPRESS, class FIGURE>
struct CBCBasedSearchTraits<RandomCBCTag<LAT, COMPRESS, FIGURE>> {
   typedef LatBuilder::Task::Search<LAT> Search;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename LatBuilder::Storage<LAT, COMPRESS>::SizeParam SizeParam;
   typedef typename FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef LFSR113 RandomGenerator;
   typedef LatBuilder::Traversal::Random<RandomGenerator> Traversal;
   typedef GenSeq::CoprimeIntegers<COMPRESS, Traversal> GenSeqType;

   CBCBasedSearchTraits(unsigned int numRand_): numRand(numRand_) {}

   virtual ~CBCBasedSearchTraits() {}

   std::vector<GenSeqType> genSeqs(const SizeParam& sizeParam, Dimension dimension)
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
      return vec;
   }

   std::string name() const
   { return FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::prefix() + "random CBC (" + boost::lexical_cast<std::string>(numRand) + " random samples)"; }

   void init(LatBuilder::Task::RandomCBC<LAT, COMPRESS, FIGURE>& search) const
   {
      FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::init(search);
      search.minObserver().setMaxAcceptedCount(numRand);
   }

   unsigned int numRand;
   RandomGenerator rand;
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, CBCBasedSearch, RandomCBC);

}}

#endif
