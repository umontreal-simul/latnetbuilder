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

#ifndef LATBUILDER__PROJ_DEP_MERIT__SPECTRAL_H
#define LATBUILDER__PROJ_DEP_MERIT__SPECTRAL_H

#include "latbuilder/ProjDepMerit/Base.h"
#include "latbuilder/Types.h"
#include "latbuilder/Storage.h"

#include "latticetester/Coordinates.h"
#include "latticetester/Rank1Lattice.h"
#include "latticetester/Reducer.h"

#include <limits>
#include <stdexcept>
#include <cmath>

namespace LatBuilder { namespace ProjDepMerit {

/**
 * Figure of merit based on the spectral test.
 *
 * Implements the computation of the inverse length of the shortest dual vector
 * in a lattice.
 *
 * The value of the figure of merit is the normalized inverse length of the
 * shortest dual vector, raised to a user-specified power.
 *
 * \tparam NORM   Normalizer type.
 */
template <class NORM>
class Spectral : public Base<Spectral<NORM>> {
public:
   typedef NORM Normalizer;

   /**
    * Constructor.
    *
    * \param power         Exponent to which the contribution for each contribution
    *                      is raised.
    */
   Spectral(
         Real power = 1.0
         ):
      Base<Spectral<NORM>>(),
      m_power(power)
   {}

   bool symmetric() const
   { return true; }

   static constexpr Compress suggestedCompression()
   { return Compress::SYMMETRIC; }

   /**
    * Returns the power parameter.
    */
   Real power() const
   { return m_power; }

   std::string name() const
   { std::ostringstream os; os << "spectral^" << power(); return os.str(); }

   template <PointSetType PST, Compress COMPRESS, PerLevelOrder PLO >
   static Storage<LatticeType::ORDINARY,PST, COMPRESS> asAcceptableStorage(Storage<LatticeType::ORDINARY,PST, COMPRESS, PLO> storage)
   {return Storage<LatticeType::ORDINARY,PST, COMPRESS>(storage.sizeParam());}

   /**
    * Creates an evaluator for the projection-dependent figure of merit.
    */
   template <PointSetType PST, Compress COMPRESS, PerLevelOrder PLO = defaultPerLevelOrder<LatticeType::ORDINARY, PST>::Order>
   Evaluator<Spectral,LatticeType::ORDINARY, PST, COMPRESS, PLO> evaluator(Storage<LatticeType::ORDINARY,PST, COMPRESS, PLO> storage) const
   { return Evaluator<Spectral,LatticeType::ORDINARY, PST, COMPRESS, PLO>(std::move(asAcceptableStorage<PST,COMPRESS,PLO>(storage)), power()); }

private:
   Real m_power;
};

namespace detail {
   template <class NORM, Compress COMPRESS, PointSetType PST>
   Real spectralEval(
            const Storage<LatticeType::ORDINARY, PointSetType::UNILEVEL, COMPRESS>& storage,
            const LatDef<LatticeType::ORDINARY, PST>& lat,
            const LatticeTester::Coordinates& projection,
            Real power
            )
   {
      typedef NORM Normalizer;

      // Ref:
      //   P. L'Ecuyer and C. Lemieux.
      //   Variance Reduction via Lattice Rules.
      //   Management Science, 46, 9 (2000), 1214-1235.
      Real logDensity = log(lat.sizeParam().numPoints());
      Normalizer normalizer(
            logDensity,
            // 1 /* lattice rank */,
            static_cast<int>(projection.size()));
      // idea: we could cache the normalizer values for each projection size
      // (check in the profiler first if this is worth it)

      if (normalizer.getNorm () != LatticeTester::L2NORM)
         // this is the L2NORM implementation
         throw std::invalid_argument ("norm of normalizer must be L2NORM");

      // if (projection.size() <= 1)
      // throw std::invalid_argument("projection order must be >= 2");

      // extract projection of the generating vector
      MVect gen(projection.size());
      {
         // first component is not used
         size_t j = 0;
         for (const auto& coord : projection){
               gen(j) = lat.gen()[coord];
                  j++;
         }   
      }

#ifdef DEBUG
      using TextStream::operator<<;
      std::cout << "      projected generator: " << gen << std::endl;
#endif

      // prepare lattice and basis reduction
      LatticeTester::Rank1Lattice lattice(
            lat.sizeParam().numPoints(),
            gen,
            static_cast<int>(projection.size()),
            normalizer.getNorm());
      lattice.buildBasis (static_cast<int>(projection.size()));
      lattice.dualize ();

      LatticeTester::Reducer reducer(lattice);

      reducer.preRedDieter((int) projection.size());

      if (not reducer.shortestVector(lattice.getNorm())) {
         // reduction failed
         return std::numeric_limits<Real>::infinity();
      }


      // get length of shortest vector under L2NORM
      lattice.updateScalL2Norm (0);

      // square length
      Real sqlength = lattice.getVecNorm(0); 

      // normalization
      Real sqlength0 =
         normalizer.getGamma(static_cast<int>(projection.size())) * std::pow(
                           lat.sizeParam().numPoints(),
               2.0 / projection.size());

      Real merit = std::sqrt (sqlength0 / sqlength);

#ifdef DEBUG
      std::cout << "      value: "
             << std::sqrt (sqlength0)
             << " / "
             << std::sqrt (sqlength)
             << " = "
             << merit << std::endl;
#endif

      return Real(pow(merit, power));
   }

   template <class NORM, Compress COMPRESS, PointSetType PST>
   RealVector spectralEval(
            const Storage<LatticeType::ORDINARY, PointSetType::MULTILEVEL, COMPRESS>& storage,
            const LatDef<LatticeType::ORDINARY, PST>& lat,
            const LatticeTester::Coordinates& projection,
            Real power
            )
   {
      RealVector out(storage.sizeParam().maxLevel() + 1, 0.0);
      auto itOut = out.begin();

      for (Level level = 0; level <= storage.sizeParam().maxLevel(); level++) {

         typename Storage<LatticeType::ORDINARY, PointSetType::UNILEVEL, COMPRESS>::SizeParam osize(storage.sizeParam().numPointsOnLevel(level));
         Storage<LatticeType::ORDINARY, PointSetType::UNILEVEL, COMPRESS> ostorage(osize);

         auto olat = createLatDef(osize, lat.gen());

         *itOut = spectralEval<NORM>(ostorage, olat, projection, power);

         ++itOut;
      }
      return out;
   }
}

/**
 * Evaluator for coordinate-uniform projeciton-dependent figures of merit.
 */
template <class NORM, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO >
class Evaluator<Spectral<NORM>,LatticeType::ORDINARY, PST, COMPRESS, PLO> {
public:
   typedef typename Storage<LatticeType::ORDINARY, PST, COMPRESS>::MeritValue MeritValue;

   Evaluator(
      Storage<LatticeType::ORDINARY, PST, COMPRESS> storage,
      Real power
      ):
      m_storage(std::move(storage)),
      m_power(std::move(power))
   {}

   /**
    * Computes the value of the figure of merit of lattice \c lat for projection
    * \c projection.
    */
   MeritValue operator() (
         const LatDef<LatticeType::ORDINARY, PST>& lat,
         const LatticeTester::Coordinates& projection
         ) const
   {
      if (projection.size() == 0)
         throw std::logic_error("Spectral: undefined for an empty projection");

#ifdef DEBUG
      using TextStream::operator<<;
      std::cout << "computing figure of merit for projection " << projection
         << " for lattice " << lat << std::endl;
#endif

      if (m_storage.sizeParam() != lat.sizeParam())
         throw std::logic_error("storage and lattice size parameters do not match");

      return detail::spectralEval<NORM>(m_storage, lat, projection, m_power);
   }

private:
   Storage<LatticeType::ORDINARY, PST, COMPRESS> m_storage;
   Real m_power;
};

}}

#endif
