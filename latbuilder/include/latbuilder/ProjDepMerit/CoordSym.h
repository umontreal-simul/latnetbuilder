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

#ifndef LATBUILDER__PROJ_DEP_MERIT__COORD_SYM_H
#define LATBUILDER__PROJ_DEP_MERIT__COORD_SYM_H

#include "latbuilder/ProjDepMerit/Base.h"
#include "latbuilder/CompressedSum.h"
#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"

#include "latcommon/Nume.h"
#include "latcommon/Coordinates.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include <sstream>
#include <cmath>
#include <stdexcept>

namespace LatBuilder { namespace ProjDepMerit {

/**
 * Coordinate-symmetric projection-dependent figure of merit.
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
class CoordSym : public Base<CoordSym<KERNEL>> {
public:
   /**
    * Constructor.
    *
    * \param kernel      Kernel \f$\omega\f$.
    */
   CoordSym(
         KERNEL kernel = KERNEL()
         ):
      Base<CoordSym<KERNEL>>(),
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

   /**
    * Creates an evaluator for the projection-dependent figure of merit.
    */
   template <LatType LAT, Compress COMPRESS>
   Evaluator<CoordSym, LAT, COMPRESS> evaluator(Storage<LAT, COMPRESS> storage) const
   { return Evaluator<CoordSym, LAT, COMPRESS>(std::move(storage), kernel().valuesVector(storage)); }

private:
   KERNEL m_kernel;
};

/**
 * Evaluator for coordinate-symmetric projeciton-dependent figures of merit.
 */
template <class KERNEL, LatType LAT, Compress COMPRESS>
class Evaluator<CoordSym<KERNEL>, LAT, COMPRESS> {
public:
   typedef typename Storage<LAT, COMPRESS>::MeritValue MeritValue;

   Evaluator(
      Storage<LAT, COMPRESS> storage,
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
         const LatDef<LAT>& lat,
         const LatCommon::Coordinates& projection
         ) const
   {
      if (projection.size() == 0)
         throw std::logic_error("CoordSym: undefined for an empty projection");

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
   Storage<LAT, COMPRESS> m_storage;
   RealVector m_kernelValues;
};

}}

#endif
