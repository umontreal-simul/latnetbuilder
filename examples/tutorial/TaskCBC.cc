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

#include "latbuilder/Task/CBC.h"
#include "latbuilder/Task/FastCBC.h"
#include "latbuilder/Task/RandomCBC.h"
#include "latbuilder/Task/Eval.h"

#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latcommon/ProductWeights.h"
#include "latbuilder/Storage.h"

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/ProjDepMerit/CoordUniform.h"
#include "latbuilder/Functor/binary.h"

#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritCombiner.h"

#include "latbuilder/TextStream.h"

#include <iostream>
#include <limits>

using namespace LatBuilder;
using TextStream::operator<<;

template <typename T, typename... ARGS>
std::unique_ptr<T> unique(ARGS&&... args)
{ return std::unique_ptr<T>(new T(std::forward<ARGS>(args)...)); }

template<LatticeType LA>
void setFilters(MeritFilterList<LA, PointSetType::UNILEVEL>& filters, const SizeParam<LA, PointSetType::UNILEVEL>& sizeParam)
{}

template<LatticeType LA>
void setFilters(MeritFilterList<LA, PointSetType::MULTILEVEL>& filters, const SizeParam<LA, PointSetType::MULTILEVEL>& sizeParam)
{ filters.add(unique<MeritCombiner::SelectLevel>(sizeParam.maxLevel())); }

template <class SEARCH>
void execute(SEARCH search)
{
   setFilters(search.filters(), search.storage().sizeParam());

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
   Storage<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::SYMMETRIC> storage(256);
   Storage<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::SYMMETRIC> estorage(storage.sizeParam().numPoints());

   execute(Task::cbc(storage, dim, figureCS()));
   execute(Task::cbc(estorage, dim, figureCS()));
   execute(Task::fastCBC(storage, dim, figureCS()));
   execute(Task::randomCBC(storage, dim, figureCS(), 5));
   execute(Task::eval(storage, dim, figureCS(), {1, 99, 27}));

   execute(Task::cbc(storage, dim, figure()));
   execute(Task::cbc(estorage, dim, figure()));

   return 0;
}
