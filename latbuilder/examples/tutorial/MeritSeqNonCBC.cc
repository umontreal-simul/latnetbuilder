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

#include "latbuilder/CoordSymFigureOfMerit.h"
#include "latcommon/ProductWeights.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latbuilder/Accumulator.h"
#include "latbuilder/Storage.h"

#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritSeq/LatSeqOverCBC.h"
#include "latbuilder/MeritSeq/CoordSymCBC.h"
#include "latbuilder/MeritSeq/CoordSymInnerProd.h"
#include "latbuilder/LatSeq/Korobov.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/GenSeq/Creator.h"

#include "latbuilder/TextStream.h"

#include <iostream>
#include <limits>

using namespace LatBuilder;
using TextStream::operator<<;

template <typename T, typename... ARGS>
std::unique_ptr<T> unique(ARGS&&... args)
{ return std::unique_ptr<T>(new T(std::forward<ARGS>(args)...)); }

template <LatType L, Compress C>
void test(const Storage<L, C>& storage, Dimension dimension)
{
   //! [figure]
   auto weights = unique<LatCommon::ProductWeights>();
   weights->setDefaultWeight(0.7);

   CoordSymFigureOfMerit<Kernel::PAlpha> figure(std::move(weights), 2);
   std::cout << "figure of merit: " << figure << std::endl;
   //! [figure]

   typedef GenSeq::CoprimeIntegers<figure.suggestedCompression()> Coprime;

   auto genSeq  = GenSeq::Creator<Coprime>::create(storage.sizeParam());
   auto genSeq0 = GenSeq::Creator<Coprime>::create(SizeParam<L>(2));

   //! [cbc]
   auto cbc = MeritSeq::cbc<MeritSeq::CoordSymInnerProd>(storage, figure);
   //! [cbc]

   //! [latSeqOverCBC]
   auto latSeqOverCBC = MeritSeq::latSeqOverCBC(cbc);
   auto latSeq = LatSeq::korobov(storage.sizeParam(), Coprime(storage.sizeParam().numPoints()), dimension);
   //! [latSeqOverCBC]

   //! [filters]
   MeritFilterList<L> filters;
   //! [filters]

   //! [search]
   auto meritSeq = latSeqOverCBC.meritSeq(latSeq);
   auto filteredSeq = filters.apply(meritSeq);
   auto best = std::min_element(filteredSeq.begin(), filteredSeq.end());
   std::cout << "BEST LATTICE: " << *best.base().base() << " with merit value " << *best << std::endl;
   //! [search]
}

int main()
{
   Dimension dim = 3;

   //! [storage]
   test(Storage<LatType::ORDINARY, Compress::SYMMETRIC>(256), dim);
   //! [storage]

   return 0;
}
