// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "latbuilder/Task/Exhaustive.h"
#include "latbuilder/Task/Random.h"
#include "latbuilder/Task/Korobov.h"
#include "latbuilder/Task/RandomKorobov.h"
#include "latbuilder/Task/Extend.h"

#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latcommon/ProductWeights.h"
#include "latbuilder/Storage.h"

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/ProjDepMerit/CoordUniform.h"
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

CoordUniformFigureOfMerit<Kernel::PAlpha> figureCS()
{
   auto weights = unique<LatCommon::ProductWeights>();
   weights->setDefaultWeight(0.7);
   return CoordUniformFigureOfMerit<Kernel::PAlpha>(std::move(weights), 2);
}

WeightedFigureOfMerit<ProjDepMerit::CoordUniform<Kernel::PAlpha>, Functor::Sum> figure()
{
   typedef ProjDepMerit::CoordUniform<Kernel::PAlpha> ProjDep;
   auto weights = unique<LatCommon::ProductWeights>();
   weights->setDefaultWeight(0.7);
   return WeightedFigureOfMerit<ProjDep, Functor::Sum>(2, std::move(weights), ProjDep(2));
}

int main()
{
   Dimension dim = 3;
   Storage<PointSetType::UNILEVEL, Compress::SYMMETRIC> storage(256);

   execute(Task::exhaustive(storage, dim, figureCS()));
   execute(Task::random(storage, dim, figureCS(), 10));
   execute(Task::korobov(storage, dim, figureCS()));
   execute(Task::randomKorobov(storage, dim, figureCS(), 10));

   execute(Task::exhaustive(storage, dim, figure()));

   Storage<PointSetType::UNILEVEL, Compress::SYMMETRIC> extStorage(storage.sizeParam().numPoints() * 2);
   execute(Task::extend(extStorage, createLatDef(storage.sizeParam(), {1, 99, 27}), figure()));

   return 0;
}
