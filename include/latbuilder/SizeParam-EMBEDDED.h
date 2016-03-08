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

#ifndef LATBUILDER__SIZE_PARAM_EMBEDDED_H
#define LATBUILDER__SIZE_PARAM_EMBEDDED_H

#include "latbuilder/SizeParam.h"

namespace LatBuilder {

/**
 * Lattice size type for embedded lattices.
 * Defining parameters:
 * - prime base for the number of points;
 * - maximum embedding level (power of the base for the last lattice in the sequence).
 */
template <>
class SizeParam<LatType::EMBEDDED> :
   public BasicSizeParam<SizeParam<LatType::EMBEDDED>> {
public:

   /**
    * Constructor.
    *
    * \param primeBase     Prime base for the number of points.
    * \param maxLevel      The last lattice in the sequence (the one with the
    *                      largest number of points) has
    *                      \f$\mathtt{primeBase}^{\mathtt{maxLevel}}\f$ points.
    */
   SizeParam(Modulus primeBase, Level maxLevel);

   /**
    * Constructor.
    *
    * \param numPoints  Number of points factorizable as an integer power of
    *                   a prime base.
    */
   SizeParam(Modulus numPoints = 0);

   template <class D>
   SizeParam(const BasicSizeParam<D>& other): SizeParam(other.numPoints())
   {}

   /**
    * Returns the prime base for the number of points.
    */
   Modulus base() const
   { return m_base; }

   /**
    * Returns the maximum embedding level of the base for the last lattice in the sequence.
    */
   Level maxLevel() const
   { return m_maxLevel; }

   /**
    * Returns the number of points for the lattice at embedding level \c level
    * in the sequence.
    */
   Modulus numPointsOnLevel(Level level) const;

   size_t totient() const;

   void normalize(Real& merit) const;

   /**
    * Divides the merit value at each level in \c merit by the number of
    * points at that level.
    */
   void normalize(RealVector& merit) const;

   std::ostream& format(std::ostream& os) const;

private:
   Modulus m_base;
   Level m_maxLevel;
};

}

#endif
