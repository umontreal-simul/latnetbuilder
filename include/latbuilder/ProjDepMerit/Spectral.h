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

#ifndef LATBUILDER__PROJ_DEP_MERIT__SPECTRAL_H
#define LATBUILDER__PROJ_DEP_MERIT__SPECTRAL_H

#include "latbuilder/ProjDepMerit/Base.h"
#include "latbuilder/Types.h"
#include "latbuilder/Storage.h"

#include "latcommon/Coordinates.h"
#include "latcommon/Rank1Lattice.h"
#include "latcommon/Reducer.h"

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

   template <LatType LAT, Compress COMPRESS, PerLvlOrder PLO >
   static Storage<Lattice::INTEGRATION,LAT, COMPRESS> asAcceptableStorage(Storage<Lattice::INTEGRATION,LAT, COMPRESS, PLO> storage)
   {return Storage<Lattice::INTEGRATION,LAT, COMPRESS>(storage.sizeParam());}

   /**
    * Creates an evaluator for the projection-dependent figure of merit.
    */
   template <LatType LAT, Compress COMPRESS, PerLvlOrder PLO = defaultPerLvlOrder<Lattice::INTEGRATION, LAT>::Order>
   Evaluator<Spectral,Lattice::INTEGRATION, LAT, COMPRESS, PLO> evaluator(Storage<Lattice::INTEGRATION,LAT, COMPRESS, PLO> storage) const
   { return Evaluator<Spectral,Lattice::INTEGRATION, LAT, COMPRESS, PLO>(std::move(asAcceptableStorage<LAT,COMPRESS,PLO>(storage)), power()); }

private:
   Real m_power;
};

namespace detail {
   template <class NORM, Compress COMPRESS, LatType LAT>
   Real spectralEval(
            const Storage<Lattice::INTEGRATION, LatType::ORDINARY, COMPRESS>& storage,
            const LatDef<Lattice::INTEGRATION, LAT>& lat,
            const LatCommon::Coordinates& projection,
            Real power
            )
   {
      typedef NORM Normalizer;

      // Ref:
      //   P. L'Ecuyer and C. Lemieux.
      //   Variance Reduction via Lattice Rules.
      //   Management Science, 46, 9 (2000), 1214-1235.
      Normalizer normalizer(
            lat.sizeParam().numPoints(),
            1 /* lattice rank */,
            static_cast<int>(projection.size()));
      // idea: we could cache the normalizer values for each projection size
      // (check in the profiler first if this is worth it)

      if (normalizer.getNorm () != LatCommon::L2NORM)
         // this is the L2NORM implementation
         throw std::invalid_argument ("norm of normalizer must be L2NORM");

      // if (projection.size() <= 1)
      // throw std::invalid_argument("projection order must be >= 2");

      // extract projection of the generating vector
      MVect gen(projection.size() + 1);
      {
         // first component is not used
         size_t j = 0;
         gen(j++) = 0;
         for (const auto& coord : projection)
            gen(j++) = lat.gen()[coord];
      }

#ifdef DEBUG
      using TextStream::operator<<;
      std::cout << "      projected generator: " << gen << std::endl;
#endif

      // prepare lattice and basis reduction
      LatCommon::Rank1Lattice lattice(
            lat.sizeParam().numPoints(),
            gen,
            static_cast<int>(projection.size()),
            normalizer.getNorm());
      lattice.buildBasis (static_cast<int>(projection.size()));
      lattice.dualize ();

      LatCommon::Reducer reducer(lattice);

      if (not reducer.shortestVector(lattice.getNorm())) {
         // reduction failed
         return std::numeric_limits<Real>::infinity();
      }

      // get length of shortest vector under L2NORM
      lattice.getPrimalBasis ().updateScalL2Norm (1);

      // square length
      Real sqlength = lattice.getPrimalBasis().getVecNorm(1);

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

   template <class NORM, Compress COMPRESS, LatType LAT>
   RealVector spectralEval(
            const Storage<Lattice::INTEGRATION, LatType::EMBEDDED, COMPRESS>& storage,
            const LatDef<Lattice::INTEGRATION, LAT>& lat,
            const LatCommon::Coordinates& projection,
            Real power
            )
   {
      RealVector out(storage.sizeParam().maxLevel() + 1, 0.0);
      auto itOut = out.begin();

      for (Level level = 0; level <= storage.sizeParam().maxLevel(); level++) {

         typename Storage<Lattice::INTEGRATION, LatType::ORDINARY, COMPRESS>::SizeParam osize(storage.sizeParam().numPointsOnLevel(level));
         Storage<Lattice::INTEGRATION, LatType::ORDINARY, COMPRESS> ostorage(osize);

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
template <class NORM, LatType LAT, Compress COMPRESS, PerLvlOrder PLO >
class Evaluator<Spectral<NORM>,Lattice::INTEGRATION, LAT, COMPRESS, PLO> {
public:
   typedef typename Storage<Lattice::INTEGRATION, LAT, COMPRESS>::MeritValue MeritValue;

   Evaluator(
      Storage<Lattice::INTEGRATION, LAT, COMPRESS> storage,
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
         const LatDef<Lattice::INTEGRATION, LAT>& lat,
         const LatCommon::Coordinates& projection
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
   Storage<Lattice::INTEGRATION, LAT, COMPRESS> m_storage;
   Real m_power;
};

}}

#endif
