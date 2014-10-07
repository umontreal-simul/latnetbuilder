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

void setFilters(MeritFilterList<LatType::ORDINARY>& filters, const SizeParam<LatType::ORDINARY>& sizeParam)
{}

void setFilters(MeritFilterList<LatType::EMBEDDED>& filters, const SizeParam<LatType::EMBEDDED>& sizeParam)
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
   Storage<LatType::ORDINARY, Compress::SYMMETRIC> storage(256);
   Storage<LatType::EMBEDDED, Compress::SYMMETRIC> estorage(storage.sizeParam().numPoints());

   execute(Task::cbc(storage, dim, figureCS()));
   execute(Task::cbc(estorage, dim, figureCS()));
   execute(Task::fastCBC(storage, dim, figureCS()));
   execute(Task::randomCBC(storage, dim, figureCS(), 5));
   execute(Task::eval(storage, dim, figureCS(), GeneratingVector{1, 99, 27}));

   execute(Task::cbc(storage, dim, figure()));
   execute(Task::cbc(estorage, dim, figure()));

   return 0;
}
