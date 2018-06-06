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

#ifndef LATBUILDER__KERNEL__BASE_H
#define LATBUILDER__KERNEL__BASE_H

#include "latbuilder/Storage.h"

#include <boost/numeric/ublas/vector.hpp>

namespace LatBuilder { namespace Kernel {

/**
 * Base base class for factories of kernel values.
 */
template <class DERIVED>
class Base {
public:
   /**
    * Creates a new vector of kernel values.
    *
    * The values of the kernel evaluated at \c sizeParam.numPoints() regular
    * intervals in \f$[0,1)\f$ are stored in a linear vector.
    * The intervals are of size \c 1/sizeParam.numPoints() and the first point is
    * at 0.
    *
    * the \f$i^{\text{th}}\f$ element  \f$\omega_i\f$ is:
    *- \f$\omega(i/n)\f$ in the case of an ordinary lattice with modulus \f$n\f$.
    *-  \f$\omega((\nu_m(\frac{i(z)}{P(z)}))\f$ in the case of a polynomial lattice of modulus \f$P(z)\f$ (\f$ i(z) = \sum a_iz^i\f$ where \f$i =\sum a_i2^i\f$).
    *
    * \return The newly created vector.
    */
   template <LatticeType LR, PointSetType L, Compress C, PerLevelOrder P > 
   RealVector valuesVector(
         const Storage<LR, L, C, P>& storage
         ) const
   { return derived().valuesVector(storage); }

   /**
    * Returns \c true if the kernel takes the same value at points \f$x\f$ and
    * \f$1 - x\f$ for \f$x \in [0,1)\f$.
    */
   bool symmetric() const
   { return derived().symmetric(); }

   /**
    * Returns the name of the kernel.
    */
   std::string name() const
   { return derived().name(); }

   DERIVED& derived()
   { return static_cast<DERIVED&>(*this); }

   const DERIVED& derived() const
   { return static_cast<const DERIVED&>(*this); }
};

/**
 * Formats \c functor and outputs it on \c os.
 */
template <class K>
std::ostream& operator<<(std::ostream& os, const Base<K>& functor)
{ return os << functor.name(); }

}}

#endif
