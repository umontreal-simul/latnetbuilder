// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
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

#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latticetester/ProductWeights.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latbuilder/Kernel/PAlphaPLR.h"
#include "latbuilder/Accumulator.h"
#include "latbuilder/Storage.h"

#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritSeq/LatSeqOverCBC.h"
#include "latbuilder/MeritSeq/CoordUniformCBC.h"
#include "latbuilder/MeritSeq/CoordUniformInnerProd.h"
#include "latbuilder/LatSeq/Korobov.h"
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/GenSeq/Creator.h"

#include "latbuilder/TextStream.h"

#include <iostream>
#include <limits>

using namespace LatBuilder;
using TextStream::operator<<;

template <typename T, typename... ARGS>
std::unique_ptr<T> unique(ARGS&&... args)
{ return std::unique_ptr<T>(new T(std::forward<ARGS>(args)...)); }

template <LatticeType LA, PointSetType L, Compress C>
void test(const Storage<LA, L, C>& storage, Dimension dimension)
{
   //! [figure]
   auto weights = unique<LatticeTester::ProductWeights>();
   weights->setDefaultWeight(0.7);

   CoordUniformFigureOfMerit<Kernel::PAlpha> figure(std::move(weights), 2);
   std::cout << "figure of merit: " << figure << std::endl;
   //! [figure]

   /*
   // The P_{\alpha,PLR} figure of merit for polynomial lattices
   //! [pfigure]
   auto weights = unique<LatticeTester::ProductWeights>();
   weights->setDefaultWeight(0.7);

   //! [pProjDepMerit]
   typedef ProjDepMerit::CoordUniform<Kernel::PAlphaPLR> ProjDep;
   WeightedFigureOfMerit<ProjDep, Functor::Sum> figure(2, std::move(weights), ProjDep(2));
   //! [pProjDepMerit]
   std::cout << "figure of merit: " << figure << std::endl;
   //! [pfigure]
   */

   typedef GenSeq::GeneratingValues<LA, decltype(figure)::suggestedCompression()> Coprime;

   auto genSeq  = GenSeq::Creator<Coprime>::create(storage.sizeParam());
   auto genSeq0 = GenSeq::Creator<Coprime>::create(SizeParam<LA, L>(LatticeTraits<LA>::TrivialModulus));

   //! [cbc]
   auto cbc = MeritSeq::cbc<MeritSeq::CoordUniformInnerProd>(storage, figure);
   //! [cbc]

   //! [latSeqOverCBC]
   auto latSeqOverCBC = MeritSeq::latSeqOverCBC(cbc);
   auto latSeq = LatSeq::korobov(storage.sizeParam(), Coprime(storage.sizeParam().modulus()), dimension);
   //! [latSeqOverCBC]

   //! [filters]
   MeritFilterList<LA, L> filters;
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
   test(Storage<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::SYMMETRIC>(256), dim);
   //! [storage]
   /*
   //! [pstorage]
   test(Storage<LatticeType::POLYNOMIAL, PointSetType::UNILEVEL, Compress::NONE>(PolynomialFromInt(115)), dim);
   //! [pstorage]
   */
   return 0;
}
