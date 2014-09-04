// Copyright (c) 2014 David Munger, Pierre L'Ecuyer, Université de Montréal.  
// Copyright (c) 2012 Richard Simard, Pierre L'Ecuyer, Université de Montréal. 
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
// 
// 
// Converted from tcode to Doxygen in 2014.

 /**
  * 
  * 
  * This class builds vectors of \c long's and is very similar to some NTL
  * classes for vectors of integers. It has been added for compatibility with NTL in
  * declaration of types and method calls. The macros used provide template-like
  * classes for dynamic-sized arrays of \c long's.
  * 
  * 
  * 
 * 
  */
 
#ifndef VEC_LZZ_H
#define VEC_LZZ_H

/**
 * Definition of type \c Long.
 * 
 * \todo Richard (hidden): Cette d\'efinition est-elle vraiment utile?
 * .
 */
#include "NTL/vector.h"
#include "NTL/vec_lzz_p.h"

typedef long Long; 
          
/**
 * This macro declares a class \c vec_zz of vectors of \c Long's.
 * The declaration  <tt>vec_zz v;</tt> creates a zero-length vector. The statement
 * \c v.SetLength(n); grows this vector to length \f$n\f$.
 * This causes space to be allocated for \f$n\f$ elements.
 * The current length of a vector is obtained by calling \c v.length().
 */
NTL_vector_decl(Long,vec_zz)

/**
 * This macro allows the use of the input and output operators
 * \f$\scriptstyle{<<}\f$ and \f$\scriptstyle{>>}\f$ for type  \c vec_zz.
 */
NTL_io_vector_decl(Long,vec_zz)

/**
 * This macro allows the use of the equality operators \f$==\f$ and \f$!=\f$ for type
 * \c vec_zz.
 */
NTL_eq_vector_decl(Long,vec_zz)

/**
 * Converts \c V into \c Vp.
 */
void conv(NTL::vec_zz_p & Vp, const vec_zz & V);

// typedef vec_zz vec_long; 


#endif

