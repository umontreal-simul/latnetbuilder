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

#ifndef LATBUILDER__PROJ_DEP_MERIT__BASE_H
#define LATBUILDER__PROJ_DEP_MERIT__BASE_H

#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"
#include "latcommon/Coordinates.h"
#include "latbuilder/Storage.h"

#include <memory>

namespace LatBuilder { namespace ProjDepMerit {

template <class DERIVED, Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO = defaultPerLvlOrder<LR, LAT>::Order>
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
   template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
   typename Storage<LR, LAT, COMPRESS, PLO>::MeritValue operator() (
         const Storage<LR, LAT, COMPRESS, PLO>& storage,
         const LatDef<LR, LAT>& lat,
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
   template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
   Evaluator<DERIVED, LR, LAT, COMPRESS, PLO> evaluator(const Storage<LR, LAT, COMPRESS, PLO>& storage) const
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
