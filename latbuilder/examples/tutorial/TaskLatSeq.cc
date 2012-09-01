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

#include "latbuilder/Task/Exhaustive.h"
#include "latbuilder/Task/Random.h"
#include "latbuilder/Task/Korobov.h"
#include "latbuilder/Task/RandomKorobov.h"
#include "latbuilder/Task/Extend.h"

#include "latbuilder/CoordSymFigureOfMerit.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latcommon/ProductWeights.h"
#include "latbuilder/Storage.h"

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/ProjDepMerit/CoordSym.h"
#include "latbuilder/Functor/binary.h"

#include "latbuilder/TextStream.h"

#include <iostream>
#include <limits>

using namespace LatBuilder;
using TextStream::operator<<;

template <typename T, typename... ARGS>
std::unique_ptr<T> unique(ARGS&&... args)
{ return std::unique_ptr<T>(new T(std::forward<ARGS>(args)...)); }

template <class SEARCH>
void execute(SEARCH search)
{
   std::cout << "------------------------------------------------------------------------" << std::endl;
   std::cout << search << std::endl;
   search.execute();

   std::cout << "BEST LATTICE: " << search.bestLattice() << ": " << search.bestMeritValue() << std::endl;
}

CoordSymFigureOfMerit<Kernel::PAlpha> figureCS()
{
   auto weights = unique<LatCommon::ProductWeights>();
   weights->setDefaultWeight(0.7);
   return CoordSymFigureOfMerit<Kernel::PAlpha>(std::move(weights), 2);
}

WeightedFigureOfMerit<ProjDepMerit::CoordSym<Kernel::PAlpha>, Functor::Sum> figure()
{
   typedef ProjDepMerit::CoordSym<Kernel::PAlpha> ProjDep;
   auto weights = unique<LatCommon::ProductWeights>();
   weights->setDefaultWeight(0.7);
   return WeightedFigureOfMerit<ProjDep, Functor::Sum>(std::move(weights), ProjDep(2));
}

int main()
{
   Dimension dim = 3;
   Storage<LatType::ORDINARY, Compress::SYMMETRIC> storage(256);

   execute(Task::exhaustive(storage, dim, figureCS()));
   execute(Task::random(storage, dim, figureCS(), 10));
   execute(Task::korobov(storage, dim, figureCS()));
   execute(Task::randomKorobov(storage, dim, figureCS(), 10));

   execute(Task::exhaustive(storage, dim, figure()));

   Storage<LatType::ORDINARY, Compress::SYMMETRIC> extStorage(storage.sizeParam().numPoints() * 2);
   execute(Task::extend(extStorage, createLatDef(storage.sizeParam(), {1, 99, 27}), figure()));

   return 0;
}
