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

#include "latbuilder/LatSeq/CBC.h"
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
   WeightedFigureOfMerit<ProjDep, Functor::Max> figure(std::move(weights));
   std::cout << "figure of merit: " << figure << std::endl;
   //! [figure]

   // sequence of lattice definitions
   typedef GenSeq::CoprimeIntegers<figure.suggestedCompression()> Coprime;
   //! [baseLat]
   auto baseLat = createLatDef(storage.sizeParam());
   //! [baseLat]

   //! [initialMerit]
   auto initialMerit = storage.createMeritValue(0.0);
   //! [initialMerit]

   auto eval = figure.evaluator(storage);

   //! [CBC loop]
   //! [CBC loop cond]
   while (baseLat.dimension() < dimension) {

      Dimension baseDim = baseLat.dimension();

      auto latSeq = LatSeq::cbc(
            baseLat,
            Coprime(baseDim == 0 ? 2 : storage.sizeParam().numPoints())
            );
   //! [CBC loop cond]

      using LatCommon::CoordinateSets::FromRanges;
      using LatCommon::CoordinateSets::AddCoordinate;

      std::cout << "CBC search for dimension: " << (baseDim + 1) << std::endl;
      std::cout << "  base lattice: " << baseLat << std::endl;
      std::cout << "  base merit value: " << initialMerit << std::endl;

      //! [newProjections]
      // base projections
      FromRanges baseProjections{
            0, baseDim,     // range for dimension
            0, baseDim - 1  // range for coordinate index
            };
      // add current coordinate to the base projections
      AddCoordinate<FromRanges> newProjections(
            baseProjections,
            baseDim         // current coordinate index
            );
      //! [newProjections]

      std::cout << "  new projections: " << newProjections << std::endl;

      //! [loop]
      Observer obs;
      for (const auto& lat : latSeq) {
         // compute merit value of lattice for all projections
         auto merit = eval(lat, newProjections, initialMerit);
         // notify the observer
         obs.observe(lat, merit);
      }
      std::cout << "BEST LATTICE: " << obs.bestLat() << " with merit value " << obs.bestMerit() << std::endl;
      //! [loop]

      //! [baseLat update]
      baseLat = obs.bestLat();
      initialMerit = obs.bestMerit();
      //! [baseLat update]
   }
   //! [CBC loop]
}

int main()
{
   Dimension dim = 3;

   //! [storage]
   test(Storage<LatType::ORDINARY, Compress::SYMMETRIC>(19), dim);
   //! [storage]

   return 0;
}
