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

#ifndef NETBUILDER__TYPES_H
#define NETBUILDER__TYPES_H

/** \file
 * Basic type definitions of NetBuilder.
 */

#include <vector>
#include <cstdint>
#include <boost/numeric/ublas/vector.hpp>
#include <functional>
#include <NTL/GF2X.h>
#include <NTL/ZZX.h>
#include "latbuilder/Types.h"


namespace NetBuilder
{

/// \defgroup ypedefsNetBuilder NetBuilder Type Definitions

//@{

/// Scalar unsigned integer .
typedef unsigned long uInteger;

/// Scalar floating-point type.
typedef double Real;

/// Vector of floating-point values.
typedef boost::numeric::ublas::vector<Real> RealVector;

/// Merit value type.
typedef Real MeritValue;

/// Scalar integer type for dimension.
typedef size_t Dimension;

// /// Type for the size of nets
// typedef size_t size_type;

/// Type of nets
typedef LatBuilder::EmbeddingType EmbeddingType;

/// Net construction methods
enum class NetConstruction {SOBOL, POLYNOMIAL, EXPLICIT, LMS};

// Level combiner for multilevel nets
// typedef std::function<Real (const RealVector&)> Combiner;

/// polynomial over Z/2Z type 
typedef LatBuilder::Polynomial Polynomial;

/// polynomial with arbitrary integers
typedef NTL::ZZX IntPolynomial;

/// Output format for nets
enum class OutputFormat {HUMAN, MACHINE};

/// Outputs Style
enum class OutputStyle {NONE, SOBOL, SOBOLJK, LATTICE, NET};


//@}
}

#endif
