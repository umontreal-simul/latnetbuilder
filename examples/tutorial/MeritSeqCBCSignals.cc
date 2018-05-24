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

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latticetester/ProductWeights.h"
#include "latticetester/CoordinateSets.h"
#include "latbuilder/ProjDepMerit/Spectral.h"
#include "latticetester/NormaBestLat.h"
#include "latbuilder/ProjDepMerit/CoordUniform.h" 
#include "latbuilder/Kernel/PAlphaPLR.h"
#include "latbuilder/Accumulator.h"
#include "latbuilder/Storage.h"
#include "latbuilder/Functor/binary.h"

#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/GenSeq/Creator.h"
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
template<LatticeType LA>
class Observer {
public:
   typedef LatBuilder::LatDef<LA, PointSetType::UNILEVEL> LatDef;

   Observer() { reset(); }

   // initializes the best observed merit value to infinity
   void reset() { m_bestMerit = std::numeric_limits<Real>::infinity(); }

   //! [onProgress]
   bool onProgress(Real merit) const
   { return merit <= m_bestMerit; }
   //! [onProgress]

   //! [onAbort]
   void onAbort(const LatDef& lat) const
   { std::cout << "rejected " << lat << std::endl; }
   //! [onAbort]

   //! [onMinUpdated]
   void onMinUpdated(const Real& bestMerit)
   { m_bestMerit = bestMerit; }
   //! [onMinUpdated]

private:
   Real m_bestMerit;
};
//! [Observer]

template <LatticeType LA, PointSetType L, Compress C>
void test(const Storage<LA, L, C>& storage, Dimension dimension)
{
   //! [figure]
   auto weights = unique<LatticeTester::ProductWeights>();
   weights->setDefaultWeight(0.7);

   typedef ProjDepMerit::Spectral<LatticeTester::NormaBestLat> ProjDep;
   WeightedFigureOfMerit<ProjDep, Functor::Max> figure(2, std::move(weights));
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
   auto cbc = MeritSeq::cbc(storage, figure);
   //! [cbc]

   //! [filters]
   MeritFilterList<LA, L> filters;
   //! [filters]

   //! [connect]
   Observer<LA> obs;
   cbc.evaluator().onProgress().connect(boost::bind(&Observer<LA>::onProgress, &obs, _1));
   cbc.evaluator().onAbort().connect(boost::bind(&Observer<LA>::onAbort, &obs, _1));
   //! [connect]

   //! [minElement]
   Functor::MinElement<Real> minElement;
   minElement.onMinUpdated().connect(boost::bind(&Observer<LA>::onMinUpdated, &obs, _1));
   //! [minElement]

   //! [CBC loop]
   while (cbc.baseLat().dimension() < dimension) {

      Dimension baseDim = cbc.baseLat().dimension();

      std::cout << "CBC search for dimension: " << (baseDim + 1) << std::endl;
      std::cout << "  base lattice: " << cbc.baseLat() << std::endl;
      std::cout << "  base merit value: " << cbc.baseMerit() << std::endl;
      std::cout << "  new projections: " << cbc.projections() << std::endl;

      //! [meritSeq]
      auto meritSeq = cbc.meritSeq(baseDim == 0 ? genSeq0 : genSeq);
      //! [meritSeq]

      //! [filteredSeq]
      auto filteredSeq = filters.apply(meritSeq);
      //! [filteredSeq]

      //! [minElement call]
      obs.reset();
      auto best = minElement(filteredSeq.begin(), filteredSeq.end());
      //! [minElement call]
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
   test(Storage<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::SYMMETRIC>(256), dim);
   //! [storage]

   /*
   //! [pstorage]
   test(Storage<LatticeType::POLYNOMIAL, PointSetType::UNILEVEL, Compress::NONE>(PolynomialFromInt(115)), dim);
   //! [pstorage]
   */

   return 0;
}
