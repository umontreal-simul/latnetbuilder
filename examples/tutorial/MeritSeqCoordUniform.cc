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

#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latcommon/ProductWeights.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latbuilder/Accumulator.h"
#include "latbuilder/Storage.h"

#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritCombiner.h"

#include "latbuilder/MeritSeq/CoordUniformCBC.h"
#include "latbuilder/MeritSeq/CoordUniformInnerProd.h"
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

void setCombiner(MeritFilterList<LatType::ORDINARY>&, const SizeParam<LatType::ORDINARY>&) {}

void setCombiner(MeritFilterList<LatType::EMBEDDED>& filters, const SizeParam<LatType::EMBEDDED>& size)
{ filters.add(unique<MeritCombiner::SelectLevel>(size.maxLevel())); }


template <LatType L, Compress C>
void test(const Storage<L, C>& storage, Dimension dimension)
{
   //! [figure]
   auto weights = unique<LatCommon::ProductWeights>();
   weights->setDefaultWeight(0.7);

   CoordUniformFigureOfMerit<Kernel::PAlpha> figure(std::move(weights), 2);
   std::cout << "figure of merit: " << figure << std::endl;
   //! [figure]

   //! [Coprime]
   typedef GenSeq::CoprimeIntegers<decltype(figure)::suggestedCompression()> Coprime;
   //! [Coprime]
   auto genSeq  = GenSeq::Creator<Coprime>::create(storage.sizeParam());
   auto genSeq0 = GenSeq::Creator<Coprime>::create(SizeParam<L>(2));

   //! [cbc]
   auto cbc = MeritSeq::cbc<MeritSeq::CoordUniformInnerProd>(storage, figure);
   //! [cbc]

   //! [filters]
   MeritFilterList<L> filters;
   setCombiner(filters, storage.sizeParam());
   //! [filters]

   //! [CBC loop]
   while (cbc.baseLat().dimension() < dimension) {

      Dimension baseDim = cbc.baseLat().dimension();

      std::cout << "CBC search for dimension: " << (baseDim + 1) << std::endl;
      std::cout << "  base lattice: " << cbc.baseLat() << std::endl;
      std::cout << "  base merit value: " << cbc.baseMerit() << std::endl;

      //! [meritSeq]
      auto meritSeq = cbc.meritSeq(baseDim == 0 ? genSeq0 : genSeq);
      //! [meritSeq]

      //! [filteredSeq]
      auto filteredSeq = filters.apply(meritSeq);
      //! [filteredSeq]

      //! [min_element]
      auto best = std::min_element(filteredSeq.begin(), filteredSeq.end());
      //! [min_element]
      //! [select]
      cbc.select(best.base());
      //! [select]

      std::cout << "BEST LATTICE: " << cbc.baseLat() << " with merit value " << *best << std::endl;
   }
   //! [CBC loop]
}

int main()
{
   Dimension dim = 3;

   //! [storage]
   test(Storage<LatType::ORDINARY, Compress::SYMMETRIC>(256), dim);
   test(Storage<LatType::EMBEDDED, Compress::SYMMETRIC>(256), dim);
   //! [storage]

   return 0;
}
