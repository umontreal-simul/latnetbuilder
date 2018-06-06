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

#ifndef LATBUILDER__FUNCTOR__R_PLR_H
#define LATBUILDER__FUNCTOR__R_PLR_H


#include "latbuilder/Types.h"
#include "latbuilder/Util.h"
#include <iostream>


#include <cmath>
#include <sstream>
#include <limits>

namespace LatBuilder { namespace Functor {

/**
 * One-dimensional merit function for the \f$\textbf{R}\f$ discrepancy.
 *
 * This merit function is defined as
 * \f[
 *    \omega(x) =
 *    \left\{
 *    \begin{array}{ll}
 *       \frac{1}{2}i_{0} & \mbox{if } x = \sum_{i=i_{0}}^{\infty} \xi_{i}2^{-i},\quad 1 \leq i_{0} \leq m,  \xi_{i_{0}} \neq 0 \\
 *        1+\frac{1}{2}m & \text{otherwise } 
 *      \end{array} \right.
 *    
 * \f]
 * where \f$m = \deg(P(z))\f$, with \f$P(z)\f$ the modulus of the polynomial lattice
 */
class RPLR {
public:
   typedef Real value_type;
   typedef Real result_type;

   /**
    * Constructor.
    *
    *
    */
   RPLR()
   { }

   

   bool symmetric() const
   { return false; }

   static constexpr Compress suggestedCompression()
   { return Compress::NONE; }

   /**
    * Returns the one-dimensional function evaluated at \c x.
    */
   result_type operator()(const value_type& x, Polynomial modulus) const{ 
    const uInteger m = deg(modulus) ;
    // std::cout << m << std::endl;
    return (x < std::numeric_limits<double>::epsilon()) ? (1 + m/2.0) : (-std::floor(std::log2(x))/2.0) ;
  }

  result_type operator()(const value_type& x, unsigned long modulus) const{ 
    const uInteger m = log2Int( (unsigned int) modulus ) ;
    // std::cout << m << std::endl;
    return (x < std::numeric_limits<double>::epsilon()) ? (1 + m/2.0) : (-std::floor(std::log2(x))/2.0) ;
  }

   std::string name() const
   { std::ostringstream os; os << "R-PLR"; return os.str(); }

private:
   
   
};

/**
 * Formats \c functor and outputs it on \c os.
 */
inline
std::ostream& operator<<(std::ostream& os, const RPLR& functor)
{ return os << functor.name(); }

}}

#endif
