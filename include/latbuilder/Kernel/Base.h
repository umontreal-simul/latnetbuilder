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
    * \return The newly created vector.
    */
   template <LatType L, Compress C> 
   RealVector valuesVector(
         const Storage<L, C>& storage
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
