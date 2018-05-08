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
#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latcommon/ProductWeights.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latbuilder/Kernel/PAlphaPLR.h"
#include "latbuilder/Accumulator.h"
#include "latbuilder/Storage.h"
#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritCombiner.h"
#include "latbuilder/MeritSeq/CoordUniformCBC.h"
#include "latbuilder/MeritSeq/CoordUniformInnerProd.h"
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
template<LatticeType LR>
void setCombiner(MeritFilterList<LR, LatEmbed::SIMPLE>&, const SizeParam<LR, LatEmbed::SIMPLE>&) {}
template<LatticeType LR>
void setCombiner(MeritFilterList<LR, LatEmbed::EMBEDDED>& filters, const SizeParam<LR, LatEmbed::EMBEDDED>& size) 
{ filters.add(unique<MeritCombiner::SelectLevel<LR>>(size.maxLevel())); }
template <LatticeType LA, LatEmbed L, Compress C>
void test(const Storage<LA, L, C>& storage, Dimension dimension)
{
//    auto weights = unique<LatCommon::ProductWeights>();
//    weights->setDefaultWeight(0.7);
//    CoordUniformFigureOfMerit<Kernel::PAlpha> figure(std::move(weights), 2);
//    std::cout << "figure of merit: " << figure << std::endl;
   
   // The P_{\alpha,PLR} figure of merit for polynomial lattices
   auto weights = unique<LatCommon::ProductWeights>();
   weights->setDefaultWeight(0.7);
   CoordUniformFigureOfMerit<Kernel::PAlphaPLR> figure(std::move(weights), 2);
   std::cout << "figure of merit: " << figure << std::endl;
   
   typedef GenSeq::GeneratingValues<LA, decltype(figure)::suggestedCompression()> Coprime;
   auto genSeq  = GenSeq::Creator<Coprime>::create(storage.sizeParam());
   auto genSeq0 = GenSeq::Creator<Coprime>::create(SizeParam<LA, L>(LatticeTraits<LA>::TrivialModulus));
   auto cbc = MeritSeq::cbc<MeritSeq::CoordUniformInnerProd>(storage, figure);
   MeritFilterList<LA, L> filters;
   setCombiner(filters, storage.sizeParam());
   while (cbc.baseLat().dimension() < dimension) {
      Dimension baseDim = cbc.baseLat().dimension();
      std::cout << "CBC search for dimension: " << (baseDim + 1) << std::endl;
      std::cout << "  base lattice: " << cbc.baseLat() << std::endl;
      std::cout << "  base merit value: " << cbc.baseMerit() << std::endl;
      auto meritSeq = cbc.meritSeq(baseDim == 0 ? genSeq0 : genSeq);
      auto filteredSeq = filters.apply(meritSeq);
      auto best = std::min_element(filteredSeq.begin(), filteredSeq.end());
      cbc.select(best.base());
      std::cout << "BEST LATTICE: " << cbc.baseLat() << " with merit value " << *best << std::endl;
   }
}
int main()
{
   Dimension dim = 3;
//    test(Storage<LatticeType::ORDINARY, LatEmbed::SIMPLE, Compress::SYMMETRIC>(256), dim);
//    test(Storage<LatticeType::ORDINARY, LatEmbed::SIMPLE, Compress::NONE>(256), dim);
   
   test(Storage<LatticeType::POLYNOMIAL, LatEmbed::SIMPLE, Compress::SYMMETRIC>(PolynomialFromInt(115)), dim);
   test(Storage<LatticeType::POLYNOMIAL, LatEmbed::SIMPLE, Compress::NONE>(PolynomialFromInt(115)), dim);
   
   return 0;
}
