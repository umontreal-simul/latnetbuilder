// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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
#include "latticetester/Coordinates.h"
#include "latbuilder/Storage.h"

#include <memory>

namespace LatBuilder { namespace ProjDepMerit {

template <class DERIVED, LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO = defaultPerLevelOrder<LR, ET>::Order>
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
   template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
   typename Storage<LR, ET, COMPRESS, PLO>::MeritValue operator() (
         const Storage<LR, ET, COMPRESS, PLO>& storage,
         const LatDef<LR, ET>& lat,
         const LatticeTester::Coordinates& projection
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
   template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
   Evaluator<DERIVED, LR, ET, COMPRESS, PLO> evaluator(const Storage<LR, ET, COMPRESS, PLO>& storage) const
   { return derived().evaluator(storage); }

   DERIVED& derived()
   { return static_cast<DERIVED&>(*this); }

   const DERIVED& derived() const
   { return static_cast<const DERIVED&>(*this); }

   Real power() const
   { return derived().power(); }

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
