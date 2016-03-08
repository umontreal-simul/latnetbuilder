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
   T result = 1;
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
 * Modular exponentiation.
 *
 * Source:
 * http://en.wikipedia.org/wiki/Modular_exponentiation#Right-to-left_binary_method
 */
Modulus modularPow(Modulus base, Modulus exponent, Modulus modulus);

/**
 * Prime factorization using the naive "trial division" algorithm.
 *
 * Returns a list of prime factors, without their multiplicity if \c raise is
 * \c false, or raised to their multiplicity if it is \c true.
 */
std::vector<Modulus> primeFactors(Modulus n, bool raise = false);

/**
 * Prime factorization using the naive "trial division" algorithm.
 *
 * Returns a map of (factor, multiplicity) pairs.
 */
std::map<Modulus, Modulus> primeFactorsMap(Modulus n);

/**
 * Extended Euclidian algorithm.
 *
 * Source:
 * http://en.wikipedia.org/wiki/Extended_Euclidean_algorithm#Iterative_method_2
 */
std::pair<long long, long long> egcd(Modulus a, Modulus b);

}

#endif
