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

#ifndef LATBUILDER__TYPES_H
#define LATBUILDER__TYPES_H

/** \file
 * Basic type definitions.
 */

#include <vector>
#include <cstdint>
#include <boost/numeric/ublas/vector.hpp>

namespace LatBuilder
{

/// \defgroup typedefs Type Definitions
//@{

/// Scalar integer type for modulus (number of points) values.
typedef uint64_t Modulus;

/// Scalar floating-point type.
typedef double Real;

/// Vector of floating-point values.
typedef boost::numeric::ublas::vector<Real> RealVector;

/// Scalar integer type for level of embedding.
typedef RealVector::size_type Level;

/// Generating vector type.
typedef std::vector<Modulus> GeneratingVector;

/// Dimension type.
typedef GeneratingVector::size_type Dimension;

/// Types of lattices.
enum class LatType { ORDINARY, EMBEDDED };

/// Types of compression.
enum class Compress { NONE, SYMMETRIC };

//@}

std::ostream& operator<<(std::ostream& os, LatType latType);

}

#endif // TYPES_LB_H
