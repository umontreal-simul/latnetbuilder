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
#include <NTL/GF2X.h>
#include "latbuilder/ntlwrap.h"


namespace LatBuilder
{

/// \defgroup typedefs Type Definitions
//@{

/// Scalar unsigned integer .
typedef unsigned long uInteger;

/// Scalar floating-point type.
typedef double Real;

/// Vector of floating-point values.
typedef boost::numeric::ublas::vector<Real> RealVector;

/// Scalar integer type for level of embedding.
typedef RealVector::size_type Level;

/// Scalar integer type for dimension.
typedef size_t Dimension;

/// polynomial over Z/2Z type 
typedef NTL::GF2X Polynomial;

/// Lattices supported: ordinary lattice rules amd polynomial lattice rules
enum class LatticeType { ORDINARY , POLYNOMIAL };

/// Simple lattice / a sequence of embedded lattices 
enum class LatEmbed { SIMPLE, EMBEDDED };

/// Types of compression.
enum class Compress { NONE, SYMMETRIC };

// Per level order for embedded lattices 
enum class PerLevelOrder {BASIC, CYCLIC};


/**
 * Lattice traits.
 *
 * Specializations of this class template must define the types:
 * - Modulus: type of the modulus of the lattice;
 * - GenValue: type of the values taken by components of the generating vector of the lattice;
 * - GeneratingVector: type for the generating vector of the lattice ;
 * \n A static const Modulus TrivialModulus : value of the modulus yielding a Lattice of one point 
 * \n and the following static functions :
 * - uInteger ToIndex(const GenValue& value) : a mapping from the set of integers (respectively polynomials) modulo the 
 *      modulus into the set of integers less than the number of points. Permits to see the integers (respectively polynomials) modulo the 
 *      modulus as indexs. Current implementation: The identity mapping for integers, and if \f$p(z) = \sum a_iz^i\f$ is a polynomial 
 *		To index(p(z)) returns \f$\sum a_i2^i\f$
 * - GenValue ToGenValue(const uInteger& index) : the inverse mapping of ToIndex
 * - uInteger NumPoints(const Modulus& modulus) given a modulus returns the number of points in the lattice point set
 * - uInteger ToKernelIndex(const size_t& index, const Modulus& modulus) For cooddinate uniform figures of merit, given an index \f$i\f$, we need 
 *			to compute \f$w(i/n)\f$ in the case of an ordinary lattice with modulus \f$n\f$, and \f$w((\nu_m(i(z)/P(z)))\f$ in the case of a polynomial 
 *			lattice of modulus \f$P(z)\f$ (\f$ i(z) = \sum a_iz^i\f$ where \f$i =\sum a_i2^i\f$). ToKernelIndex computes an integer \f$x\f$ such that the required quantity is \f$w(x/n)\f$, where \f$n\f$ is the number of points. i.e. 
 *			\f$x = i\f$ in the case of an ordinary lattice and \f$x = n\nu_m(i(z)/P(z))\f$ for polynomial lattices.
 */
template <LatticeType LR>
struct LatticeTraits;

/**
 * Lattice traits for ordinary lattice rule.
 *
 */
template<>
struct LatticeTraits<LatticeType::ORDINARY>{
	/// Scalar integer type for modulus (= number of points) values.
	typedef uInteger Modulus;
	/// Scalar integer type for genarating values.
	typedef uInteger GenValue;
	/// Generating vector type.
	typedef std::vector<GenValue> GeneratingVector;
	

	static const Modulus TrivialModulus = 2; 
	static uInteger ToIndex(const GenValue& value);
	static GenValue ToGenValue(const uInteger& index);
	static uInteger NumPoints(const Modulus& modulus);
	static uInteger ToKernelIndex(const size_t& index, const Modulus& modulus);
};

/**
 * Lattice traits for polynomial lattice rule.
 *
 */
template<>
struct LatticeTraits<LatticeType::POLYNOMIAL>{
	/// type for modulus  values.
	typedef Polynomial Modulus;
	/// type for genarating values.
	typedef Polynomial GenValue;
	/// Generating vector type.
	typedef std::vector<GenValue> GeneratingVector;
	/// Dimension type.
	typedef GeneratingVector::size_type Dimension;

	static const Modulus TrivialModulus ; // TrivialModulus(z) = z
	static uInteger ToIndex(const GenValue& value) ;
	static GenValue ToGenValue(const uInteger& index) ;
	static uInteger NumPoints(const Modulus& modulus);
	static uInteger ToKernelIndex(const size_t& index, const Modulus& modulus);
};


//@}
std::ostream& operator<<(std::ostream& os, LatEmbed latType);


}

#endif // TYPES_LB_H
