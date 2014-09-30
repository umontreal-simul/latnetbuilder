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

   /**
    * Creates an evaluator for the projection-dependent figure of merit.
    */
   template <LatType LAT, Compress COMPRESS>
   Evaluator<Spectral, LAT, COMPRESS> evaluator(Storage<LAT, COMPRESS> storage) const
   { return Evaluator<Spectral, LAT, COMPRESS>(std::move(storage), power()); }

private:
   Real m_power;
};

namespace detail {
   template <class NORM, Compress COMPRESS, LatType LAT>
   Real spectralEval(
            const Storage<LatType::ORDINARY, COMPRESS>& storage,
            const LatDef<LAT>& lat,
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
            const Storage<LatType::EMBEDDED, COMPRESS>& storage,
            const LatDef<LAT>& lat,
            const LatCommon::Coordinates& projection,
            Real power
            )
   {
      RealVector out(storage.sizeParam().maxLevel() + 1, 0.0);
      auto itOut = out.begin();

      for (Level level = 0; level <= storage.sizeParam().maxLevel(); level++) {

         typename Storage<LatType::ORDINARY, COMPRESS>::SizeParam osize(storage.sizeParam().numPointsOnLevel(level));
         Storage<LatType::ORDINARY, COMPRESS> ostorage(osize);

         auto olat = createLatDef(osize, lat.gen());

         *itOut = spectralEval<NORM>(ostorage, olat, projection, power);

         ++itOut;
      }
      return out;
   }
}

/**
 * Evaluator for coordinate-symmetric projeciton-dependent figures of merit.
 */
template <class NORM, LatType LAT, Compress COMPRESS>
class Evaluator<Spectral<NORM>, LAT, COMPRESS> {
public:
   typedef typename Storage<LAT, COMPRESS>::MeritValue MeritValue;

   Evaluator(
      Storage<LAT, COMPRESS> storage,
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
         const LatDef<LAT>& lat,
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
   Storage<LAT, COMPRESS> m_storage;
   Real m_power;
};

}}

#endif
