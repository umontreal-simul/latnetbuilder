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

#ifndef LATBUILDER__SIZE_PARAM_ORDINARY_H
#define LATBUILDER__SIZE_PARAM_ORDINARY_H

#include "latbuilder/SizeParam.h"

namespace LatBuilder {

/**
 * Ordinary lattice size parameter.
 */
template <>
class SizeParam<LatType::ORDINARY> :
   public BasicSizeParam<SizeParam<LatType::ORDINARY>> {
public:
   SizeParam(Modulus numPoints = 0);

   template <LatType L>
   SizeParam(const SizeParam<L>& other): SizeParam(other.numPoints())
   {}

   /**
    * Returns the value of Euler's totient function.
    * It is the number of positive integers that are smaller than and coprime
    * to the number of points.
    */
   size_t totient() const;

   /**
    * Divides the merit value \c merit by the number of points.
    */
   void normalize(Real& merit) const;

   /**
    * Divides the each merit value in \c merit by the number of points.
    */
   void normalize(RealVector& merit) const;

   std::ostream& format(std::ostream& os) const;
};

}

#endif
