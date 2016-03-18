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
#include "latcommon/ProductWeights.h"
#include "latcommon/CoordinateSets.h"

#include "latbuilder/ProjDepMerit/Spectral.h"
#include "latcommon/NormaBestLat.h"

#include "latbuilder/Accumulator.h"
#include "latbuilder/Storage.h"
#include "latbuilder/Functor/binary.h"

#include "latbuilder/LatSeq/Korobov.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"

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

   // returns the best observed lattice
   const LatDef& bestLat() { return m_bestLat; }

   // returns the best observed merit value
   const Real bestMerit() { return m_bestMerit; }

   // notifies the observer that the merit value of a new candidate lattice has
   // been observed; updates the best observed candidate lattice if necessary
   void observe(const LatDef& lat, Real merit)
   {
      std::cout << lat << "\t:\t" << merit;
      if (merit < m_bestMerit) {
         std::cout << " <-- best";
         m_bestMerit = merit;
         m_bestLat = lat;
      }
      std::cout << std::endl;
   }

   //! [onProgress]
   bool onProgress(Real merit) const
   { return merit < m_bestMerit; }
   //! [onProgress]

   //! [onAbort]
   void onAbort(const LatDef& lat) const
   { std::cout << "rejected " << lat << std::endl; }
   //! [onAbort]

private:
   LatDef m_bestLat;
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

   // sequence of lattice definitions
   typedef GenSeq::CoprimeIntegers<decltype(figure)::suggestedCompression()> Coprime;
   auto latSeq = LatSeq::korobov(
         storage.sizeParam(),
         Coprime(storage.sizeParam().numPoints()),
         dimension
         );

   //! [allProjections]
   LatCommon::CoordinateSets::FromRanges allProjections(
         1, latSeq.latDimension(),     // range for dimension
         0, latSeq.latDimension() - 1  // range for coordinate index
         );
   std::cout << "projections: " << allProjections << std::endl;
   //! [allProjections]

   //! [initialMerit]
   auto initialMerit = storage.createMeritValue(0.0);
   //! [initialMerit]

   auto eval = figure.evaluator(storage);

   //! [connect]
   Observer obs;
   eval.onProgress().connect(boost::bind(&Observer::onProgress, &obs, _1));
   eval.onAbort().connect(boost::bind(&Observer::onAbort, &obs, _1));
   //! [connect]

   //! [loop]
   for (const auto& lat : latSeq) {
      // compute merit value of lattice for all projections
      auto merit = eval(lat, allProjections, initialMerit);
      // notify the observer
      obs.observe(lat, merit);
   }
   std::cout << "BEST LATTICE: " << obs.bestLat() << " with merit value " << obs.bestMerit() << std::endl;
   //! [loop]
}

int main()
{
   Dimension dim = 3;

   //! [storage]
   test(Storage<LatType::ORDINARY, Compress::SYMMETRIC>(19), dim);
   //! [storage]

   return 0;
}
