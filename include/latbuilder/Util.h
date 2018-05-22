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

/**
 * \file
 * Tools for streaming and poor man's factorization.
 */

#ifndef LATBUILDER__UTIL_H
#define LATBUILDER__UTIL_H

#include "latbuilder/Types.h"

#include <map>
#include <vector>

//================================================================================

namespace LatBuilder
{

//================================================================================

/**
 * Integer exponentiation.
 *
 * Source:
 * http://en.wikipedia.org/wiki/Exponentiation_by_squaring#Computation_by_powers_of_2
 */
template <typename T>
T intPow(T base, unsigned long exponent)
{
   T result = (T) (1);
   while (exponent) {
      if (exponent % 2 == 1)
         result *= base;
      base *= base;
      exponent /= 2;
   }
   return result;
}

//================================================================================

/**
 * convert Integer to polynomial  
 *
 * the integer \f$\sum a_i2^i\f$ is converted to \f$ \sum a_iz^i\f$
 * Note that the input integer must be < 2^64
 */
Polynomial PolynomialFromInt(uInteger x);

/**
 * convert polynomial to integer 
 *
 * the polyomial \f$ \sum a_iz^i\f$ is converted to \f$\sum a_i2^i\f$
 * 
 */
uInteger IndexOfPolynomial(Polynomial P);
/**
 * Modular exponentiation.
 *
 * Source:
 * http://en.wikipedia.org/wiki/Modular_exponentiation#Right-to-left_binary_method
 *Note : to use this fonction an operator %  
 * BASE operator% (BASE, MODULUS) 
 * has to be implemented 
 */
template<typename T>
T modularPow(T base, uInteger exponent, T modulus)
{
   T result = (T)(1);
   while (exponent) {
      if (exponent % 2 == 1)
         result = (result * base) % modulus;
      exponent /= 2;
      base = (base * base) % modulus;
   }
   return result;
}

/**
 * Prime factorization using the naive "trial division" algorithm.
 *
 * Returns a list of prime factors, without their multiplicity if \c raise is
 * \c false, or raised to their multiplicity if it is \c true.
 */
std::vector<uInteger> primeFactors(uInteger n, bool raise = false);

/**
 * Prime factorization using the naive "trial division" algorithm.
 *
 * Returns a map of (factor, multiplicity) pairs.
 */
std::map<uInteger, uInteger> primeFactorsMap(uInteger n);

/**
 * Extended Euclidian algorithm.
 *
 * Source:
 * http://en.wikipedia.org/wiki/Extended_Euclidean_algorithm#Iterative_method_2
 */
std::pair<long long, long long> egcd(uInteger a, uInteger b);

/**
 * computes The integer \f$2^{\deg(P)}\nu_m(\frac{h} {p}) \f$ \n
 * where \f$\nu_{m}\f$ is the mapping \f$\nu_{m} : \mathbb{L}_{2} \rightarrow \mathbb{R} \f$ 
 *     \f[
 *    \nu_{m}(\sum_{l=\omega}^{\infty} x_{l} z^{-l}) = \sum_{l=\max(\omega,1)}^{m} x_{l} 2^{-l}
 *     \f]
 *
 * 
 * reference : https://people.cs.kuleuven.be/~dirk.nuyens/phdthesis/thesis.pdf page 153
 */
uInteger Vm(const Polynomial& h, const Polynomial& P);

uInteger log2Int(unsigned int n);

}

#endif
