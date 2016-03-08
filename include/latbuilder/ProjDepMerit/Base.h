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

#ifndef LATBUILDER__PROJ_DEP_MERIT__BASE_H
#define LATBUILDER__PROJ_DEP_MERIT__BASE_H

#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"
#include "latcommon/Coordinates.h"
#include "latbuilder/Storage.h"

#include <memory>

namespace LatBuilder { namespace ProjDepMerit {

template <class DERIVED, LatType LAT, Compress COMPRESS>
class Evaluator;

/**
 * Base base class for projection-dependent figures of merit.
 */
template <class DERIVED>
class Base {
public:
   /**
    * Computes the value of the figure of merit of lattice \c lat for projection
    * \c projection.
    */
   template <LatType LAT, Compress COMPRESS>
   typename Storage<LAT, COMPRESS>::MeritValue operator() (
         const Storage<LAT, COMPRESS>& storage,
         const LatDef<LAT>& lat,
         const LatCommon::Coordinates& projection
         ) const
   { return derived()(storage, lat, projection); }

   /**
    * Returns the name of the figure of merit.
    */
   std::string name() const
   { return derived().name(); }

   /**
    * Returns \c true if the value of the figure of merit is invariant under a
    * reflection of the generating vector \f$\boldsymbol a=(a_1, \dots, a_s)\f$
    * along any axis such that \f$a_j \mapsto n - a_j\f$, where \f$n\f$ is the
    * number of points in the lattice point set.
    */
   bool symmetric() const
   { return derived().symmetric(); }

   /**
    * Creates an evaluator for the projection-dependent figure of merit.
    */
   template <LatType LAT, Compress COMPRESS>
   Evaluator<DERIVED, LAT, COMPRESS> evaluator(const Storage<LAT, COMPRESS>& storage) const
   { return derived().evaluator(storage); }

   DERIVED& derived()
   { return static_cast<DERIVED&>(*this); }

   const DERIVED& derived() const
   { return static_cast<const DERIVED&>(*this); }

private:
   template <class D>
   friend std::ostream& operator<<(std::ostream&, const Base<D>&);
};

/**
 * Formats \c merit and outputs it on \c os.
 */
template <class D>
std::ostream& operator<<(std::ostream& os, const Base<D>& merit)
{
   os << merit.name();
   if (merit.symmetric())
      os << " (symmetric)";
   return os;
}

}}

#endif
