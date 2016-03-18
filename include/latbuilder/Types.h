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
typedef unsigned long Modulus;

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
