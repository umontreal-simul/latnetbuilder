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
#include "latbuilder/MeritFilter.h"
#include "latbuilder/Norm/Normalizer.h"
#include "latbuilder/Norm/PAlphaSL10.h"
#include "latbuilder/Functor/LowPass.h"

#include "latbuilder/MeritSeq/CoordUniformCBC.h"
#include "latbuilder/MeritSeq/CoordUniformInnerProd.h"
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/GenSeq/Creator.h"

#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR258.h"
#include "latbuilder/Functor/MinElement.h"

#include "latbuilder/TextStream.h"

#include <iostream>
#include <limits>

using namespace LatBuilder;
using TextStream::operator<<;

template <typename T, typename... ARGS>
std::unique_ptr<T> unique(ARGS&&... args)
{ return std::unique_ptr<T>(new T(std::forward<ARGS>(args)...)); }

//! [Observer]
template <LatticeType LA, EmbeddingType ET>
class Observer {
public:
   typedef LatBuilder::LatDef<LA, ET> LatDef;

   Observer(int maxCount) { m_maxCount = maxCount; m_count = m_totalCount = 0; }
   void onStart() { m_count = m_totalCount = 0; }
   bool onElementVisited(const Real&) { ++m_totalCount; return ++m_count < m_maxCount; }
   void onReject(const LatDef&) { m_count--; }
   int count() const { return m_count; }
   int totalCount() const { return m_totalCount; }
private:
   int m_maxCount;
   int m_count;
   int m_totalCount;
};
//! [Observer]


template <LatticeType LA, EmbeddingType L, Compress C>
void test(const Storage<LA, L, C>& storage, Dimension dimension, int samples)
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

   CoordUniformFigureOfMerit<Kernel::PAlphaPLR> figure(std::move(weights), 2);
   std::cout << "figure of merit: " << figure << std::endl;
   //! [pfigure]
   */


   //! [Coprime]
   typedef GenSeq::GeneratingValues<LA, decltype(figure)::suggestedCompression(), Traversal::Random<LFSR258>> Coprime;
   auto genSeq  = GenSeq::Creator<Coprime>::create(storage.sizeParam());
   //! [Coprime]
   auto genSeq0 = GenSeq::Creator<Coprime>::create(SizeParam<LA,L>(LatticeTraits<LA>::TrivialModulus));

   //! [cbc]
   auto cbc = MeritSeq::cbc<MeritSeq::CoordUniformInnerProd>(storage, figure);
   //! [cbc]

   //! [filters]
   MeritFilterList<LA, L> filters;

   //! [normalizer]
   auto normalizer = unique<Norm::Normalizer<LA, L, Norm::PAlphaSL10>>(
         Norm::PAlphaSL10(figure.kernel().alpha(), figure.weights())
         );
   filters.add(std::move(normalizer));
   //! [normalizer]

   //! [low-pass]
   auto lowPass = unique<MeritFilter<LA, L>>(Functor::LowPass<Real>(1.0), "low-pass");
   filters.add(std::move(lowPass));
   //! [low-pass]
   std::cout << "filters: " << filters << std::endl;
   //! [filters]

   //! [minElement]
   Functor::MinElement<Real> minElement;
   //! [minElement]

   //! [signals]
   Observer<LA, L> obs(samples);
   minElement.onStart().connect(boost::bind(&Observer<LA, L>::onStart, &obs));
   minElement.onElementVisited().connect(boost::bind(&Observer<LA, L>::onElementVisited, &obs, _1));
   filters.template onReject<L>().connect(boost::bind(&Observer<LA, L>::onReject, &obs, _1));
   //! [signals]

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
      auto best = minElement(filteredSeq.begin(), filteredSeq.end(), 0);
      //! [min_element]
      //! [select]
      cbc.select(best.base());
      //! [select]

      //! [output]
      std::cout << "BEST LATTICE: " << std::endl << cbc.baseLat() << "Merit value: " << *best << std::endl;
      std::cout << obs.count() << " accepted / " << obs.totalCount() << " tried" << std::endl;;
      //! [output]

      // use a different permutation for the next iteration
      genSeq.randomGenerator().jump();
   }
   //! [CBC loop]
}

int main()
{
   Dimension dim = 3;
   int samples = 15;
   //! [storage]
   test(Storage<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC>(257), dim, samples);
   //! [storage]
   /*
   //! [pstorage]
   test(Storage<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE>(PolynomialFromInt(115)), dim, samples);
   //! [pstorage]
   */
   return 0;
}
