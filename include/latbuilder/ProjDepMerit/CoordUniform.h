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

#ifndef LATBUILDER__PROJ_DEP_MERIT__COORD_SYM_H
#define LATBUILDER__PROJ_DEP_MERIT__COORD_SYM_H

#include "latbuilder/ProjDepMerit/Base.h"
#include "latbuilder/CompressedSum.h"
#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"

#include "latticetester/Num.h"
#include "latticetester/Coordinates.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include <sstream>
#include <cmath>
#include <stdexcept>

namespace LatBuilder { namespace ProjDepMerit {

/**
 * Coordinate-uniform projection-dependent figure of merit.
 *
 * This type of projection-dependent figure of merit is base on a kernel
 * \f$\omega\f$ such that, for number of points \f$n\f$ and generating vector
 * \f$\boldsymbol a = (a_1, \dots, a_s)\f$, the merit value for a projection on
 * coordinates in \f$\mathfrak u\f$ is
 * \f[
 *   \frac1n \sum_{i=0}^{n-1}
 *   \prod_{j \in \mathfrak u} \omega((i a_j / n) \bmod 1)
 * \f]
 *
 * \tparam KERNEL Kernel \f$\omega\f$.
 */
template <class KERNEL>
class CoordUniform : public Base<CoordUniform<KERNEL>> {
public:
   /**
    * Constructor.
    *
    * \param kernel      Kernel \f$\omega\f$.
    */
   CoordUniform(
         KERNEL kernel = KERNEL()
         ):
      Base<CoordUniform<KERNEL>>(),
      m_kernel(std::move(kernel))
   {}

   bool symmetric() const
   { return kernel().symmetric(); }

   static constexpr Compress suggestedCompression()
   { return KERNEL::suggestedCompression(); }

   const KERNEL& kernel() const
   { return m_kernel; }

   std::string name() const
   { return kernel().name(); }

   Real power() const
   { return KERNEL::CUPower; }

   /**
    * Creates an evaluator for the projection-dependent figure of merit.
    */
   template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
   Evaluator<CoordUniform, LR, ET, COMPRESS, PLO> evaluator(Storage<LR, ET, COMPRESS, PLO> storage) const
   { return Evaluator<CoordUniform, LR, ET, COMPRESS, PLO>(std::move(storage), kernel().valuesVector(storage)); }

private:
   KERNEL m_kernel;
};

/**
 * Evaluator for coordinate-uniform projeciton-dependent figures of merit.
 */
template <class KERNEL, LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
class Evaluator<CoordUniform<KERNEL>, LR, ET, COMPRESS, PLO> {
public:
   typedef typename Storage<LR, ET, COMPRESS, PLO>::MeritValue MeritValue;

   Evaluator(
      Storage<LR, ET, COMPRESS, PLO> storage,
      RealVector kernelValues
      ):
      m_storage(std::move(storage)),
      m_kernelValues(std::move(kernelValues))
   {}

   /**
    * Computes the value of the figure of merit of lattice \c lat for projection
    * \c projection.
    */
   MeritValue operator() (
         const LatDef<LR, ET>& lat,
         const LatticeTester::Coordinates& projection
         ) const
   {
      if (projection.size() == 0)
         throw std::logic_error("CoordUniform: undefined for an empty projection");

#ifdef DEBUG
      using TextStream::operator<<;
      std::cout << "computing figure of merit for projection " << projection
         << " for lattice " << lat << std::endl;
#endif

      if (m_storage.sizeParam() != lat.sizeParam())
         throw std::logic_error("storage and lattice size parameters do not match");

      RealVector prod(m_storage.size(), 1.0);

      for (const auto coord : projection) {
         const auto stridedKernel = m_storage.strided(m_kernelValues, lat.gen()[coord]);
         prod = boost::numeric::ublas::element_prod(prod, stridedKernel);
      }

      auto merit = compressedSum(m_storage, prod);

      lat.sizeParam().normalize(merit);

#ifdef DEBUG
      std::cout << "  merit value: " << merit << std::endl;
#endif

      return merit;
   }

private:
   Storage<LR, ET, COMPRESS, PLO> m_storage;
   RealVector m_kernelValues;
};

}}

#endif
