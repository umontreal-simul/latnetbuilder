// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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
#include "latbuilder/Kernel/PAlphaTilde.h"
#include "latbuilder/Accumulator.h"
#include "latbuilder/Storage.h"

#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritCombiner.h"

#include "latbuilder/MeritSeq/CoordUniformCBC.h"
#include "latbuilder/MeritSeq/CoordUniformInnerProdFast.h"
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/GenSeq/Creator.h"

#include "latbuilder/TextStream.h"

#include "Path.h"

#include <iostream>
#include <limits>

using namespace LatBuilder;
using TextStream::operator<<;

template <typename T, typename... ARGS>
std::unique_ptr<T> unique(ARGS&&... args)
{ return std::unique_ptr<T>(new T(std::forward<ARGS>(args)...)); }

template<LatticeType LR>
void setCombiner(MeritFilterList<LR, EmbeddingType::UNILEVEL>&, const SizeParam<LR, EmbeddingType::UNILEVEL>&) {}

template<LatticeType LR>
void setCombiner(MeritFilterList<LR, EmbeddingType::MULTILEVEL>& filters, const SizeParam<LR, EmbeddingType::MULTILEVEL>& size) 
{ filters.add(unique<MeritCombiner::SelectLevel<LR>>(size.maxLevel())); }


template <LatticeType LA, EmbeddingType L, Compress C>
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
   typedef ProjDepMerit::CoordUniform<Kernel::PAlphaTilde> ProjDep;
   WeightedFigureOfMerit<ProjDep, Functor::Sum> figure(2, std::move(weights), ProjDep(2));
   //! [pProjDepMerit]
   std::cout << "figure of merit: " << figure << std::endl;
   //! [pfigure]
   */

   //! [Coprime]
   typedef GenSeq::CyclicGroup<LA, decltype(figure)::suggestedCompression()> Coprime;
   //! [Coprime]
   auto genSeq  = GenSeq::Creator<Coprime>::create(storage.sizeParam());
   auto genSeq0 = GenSeq::Creator<Coprime>::create(SizeParam<LA, L>(LatticeTraits<LA>::TrivialModulus));

   //! [cbc]
   auto cbc = MeritSeq::cbc<MeritSeq::CoordUniformInnerProdFast>(storage, figure);
   //! [cbc]

   //! [filters]
   MeritFilterList<LA, L> filters;
   setCombiner(filters, storage.sizeParam());
   //! [filters]

   //! [CBC loop]
   while (cbc.baseLat().dimension() < dimension) {

      Dimension baseDim = cbc.baseLat().dimension();

      std::cout << "CBC search for dimension: " << (baseDim + 1) << std::endl;
      std::cout << "base lattice: " << std::endl << cbc.baseLat();
      std::cout << "base merit value: " << cbc.baseMerit() << std::endl;

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

      std::cout << "BEST LATTICE: " << std::endl << cbc.baseLat() << "Merit value: " << *best << std::endl;
   }
   //! [CBC loop]
}

int main()
{
   SET_PATH_TO_LATNETBUILDER_FOR_EXAMPLES();
   Dimension dim = 3;
   
   //! [storage]
   test(Storage<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC>(256), dim);
   test(Storage<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::SYMMETRIC>(256), dim);
   //! [storage]
   
   /*
   //! [pstorage]
   test(Storage<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE>(PolynomialFromInt(115)), dim);
   test(Storage<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE>(PolynomialFromInt(115)), dim);
   //! [pstorage]
   */
   

   return 0;
}
