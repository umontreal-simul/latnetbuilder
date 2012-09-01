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

#include "latbuilder/CoordSymFigureOfMerit.h"
#include "latcommon/ProductWeights.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latbuilder/Accumulator.h"
#include "latbuilder/Storage.h"

#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritFilter.h"
#include "latbuilder/Norm/Normalizer.h"
#include "latbuilder/Norm/PAlphaSL10.h"
#include "latbuilder/Functor/LowPass.h"

#include "latbuilder/MeritSeq/CoordSymCBC.h"
#include "latbuilder/MeritSeq/CoordSymInnerProd.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
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
template <LatType LAT>
class Observer {
public:
   typedef LatBuilder::LatDef<LAT> LatDef;

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


template <LatType L, Compress C>
void test(const Storage<L, C>& storage, Dimension dimension, int samples)
{
   //! [figure]
   auto weights = unique<LatCommon::ProductWeights>();
   weights->setDefaultWeight(0.7);

   CoordSymFigureOfMerit<Kernel::PAlpha> figure(std::move(weights), 2);
   std::cout << "figure of merit: " << figure << std::endl;
   //! [figure]

   //! [Coprime]
   typedef GenSeq::CoprimeIntegers<figure.suggestedCompression(), Traversal::Random<LFSR258>> Coprime;
   auto genSeq  = GenSeq::Creator<Coprime>::create(storage.sizeParam());
   //! [Coprime]
   auto genSeq0 = GenSeq::Creator<Coprime>::create(SizeParam<L>(2));

   //! [cbc]
   auto cbc = MeritSeq::cbc<MeritSeq::CoordSymInnerProd>(storage, figure);
   //! [cbc]

   //! [filters]
   MeritFilterList<L> filters;

   //! [normalizer]
   auto normalizer = unique<Norm::Normalizer<L, Norm::PAlphaSL10>>(
         Norm::PAlphaSL10(figure.kernel().alpha(), figure.weights())
         );
   filters.add(std::move(normalizer));
   //! [normalizer]

   //! [low-pass]
   auto lowPass = unique<MeritFilter<L>>(Functor::LowPass<Real>(1.0), "low-pass");
   filters.add(std::move(lowPass));
   //! [low-pass]
   std::cout << "filters: " << filters << std::endl;
   //! [filters]

   //! [minElement]
   Functor::MinElement<Real> minElement;
   //! [minElement]

   //! [signals]
   Observer<L> obs(samples);
   minElement.onStart().connect(boost::bind(&Observer<L>::onStart, &obs));
   minElement.onElementVisited().connect(boost::bind(&Observer<L>::onElementVisited, &obs, _1));
   filters.template onReject<L>().connect(boost::bind(&Observer<L>::onReject, &obs, _1));
   //! [signals]

   //! [CBC loop]
   while (cbc.baseLat().dimension() < dimension) {

      Dimension baseDim = cbc.baseLat().dimension();

      std::cout << "CBC search for dimension: " << (baseDim + 1) << std::endl;
      std::cout << "  base lattice: " << cbc.baseLat() << std::endl;
      std::cout << "  base merit value: " << cbc.baseMerit() << std::endl;

      //! [meritSeq]
      auto meritSeq = cbc.meritSeq(baseDim == 0 ? genSeq0 : genSeq);
      //! [meritSeq]

      //! [filteredSeq]
      auto filteredSeq = filters.apply(meritSeq);
      //! [filteredSeq]

      //! [min_element]
      auto best = minElement(filteredSeq.begin(), filteredSeq.end());
      //! [min_element]
      //! [select]
      cbc.select(best.base());
      //! [select]

      //! [output]
      std::cout << "BEST LATTICE: " << cbc.baseLat() << " with merit value " << *best << std::endl;
      std::cout << "  " << obs.count() << " accepted / " << obs.totalCount() << " tried" << std::endl;;
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
   test(Storage<LatType::ORDINARY, Compress::SYMMETRIC>(257), dim, samples);
   //! [storage]
   return 0;
}
