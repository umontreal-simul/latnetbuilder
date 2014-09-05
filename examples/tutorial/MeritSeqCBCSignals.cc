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

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latcommon/ProductWeights.h"
#include "latcommon/CoordinateSets.h"
#include "latbuilder/ProjDepMerit/Spectral.h"
#include "latcommon/NormaBestLat.h"
#include "latbuilder/Accumulator.h"
#include "latbuilder/Storage.h"
#include "latbuilder/Functor/binary.h"

#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
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
class Observer {
public:
   typedef LatBuilder::LatDef<LatType::ORDINARY> LatDef;

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

template <LatType L, Compress C>
void test(const Storage<L, C>& storage, Dimension dimension)
{
   //! [figure]
   auto weights = unique<LatCommon::ProductWeights>();
   weights->setDefaultWeight(0.7);

   typedef ProjDepMerit::Spectral<LatCommon::NormaBestLat> ProjDep;
   WeightedFigureOfMerit<ProjDep, Functor::Max> figure(2, std::move(weights));
   std::cout << "figure of merit: " << figure << std::endl;
   //! [figure]

   typedef GenSeq::CoprimeIntegers<decltype(figure)::suggestedCompression()> Coprime;
   auto genSeq  = GenSeq::Creator<Coprime>::create(storage.sizeParam());
   auto genSeq0 = GenSeq::Creator<Coprime>::create(SizeParam<L>(2));

   //! [cbc]
   auto cbc = MeritSeq::cbc(storage, figure);
   //! [cbc]

   //! [filters]
   MeritFilterList<L> filters;
   //! [filters]

   //! [connect]
   Observer obs;
   cbc.evaluator().onProgress().connect(boost::bind(&Observer::onProgress, &obs, _1));
   cbc.evaluator().onAbort().connect(boost::bind(&Observer::onAbort, &obs, _1));
   //! [connect]

   //! [minElement]
   Functor::MinElement<Real> minElement;
   minElement.onMinUpdated().connect(boost::bind(&Observer::onMinUpdated, &obs, _1));
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
   test(Storage<LatType::ORDINARY, Compress::SYMMETRIC>(256), dim);
   //! [storage]

   return 0;
}
