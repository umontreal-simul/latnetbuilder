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
 * This class builds rectangular matrices of \c long's and is very similar
 * to some NTL classes for matrices of integers. It has been added for compatibility
 * with NTL in declaration of types and method calls. The macros used provide
 * template-like classes for dynamic-sized rectangular matrices of \c long's.
 * 
 * 
 * 
 */

#ifndef VEC_VEC_LZZ_H
#define VEC_VEC_LZZ_H

/**
 * Definition of a vector of \c Long's (see module \c vec_lzz.tex).
 * 
 * \todo Richard (hidden): Cette d\'efinition est-elle vraiment utile?
 * Pourrait-\^etre \'elimin\'ee, je crois
 * .
 */
#include "latcommon/vec_lzz.h"


typedef vec_zz Vec; 
          
/**
 * This macro declares a class \c vec_vec_zz of rectangular matrices of
 * \c Long's (see module \c vec_lzz.tex).
 * The declaration <tt>vec_vec_zz M;</tt> creates a \f$0\times0\f$
 * matrix \c M. The statement <tt>M.SetDims(2, 4);</tt> grows this matrix
 * to dimensions \f$2\times 4\f$. A matrix entry can be accessed as \c M[i][j].
 * Indexing starts at 0.
 */
NTL_vector_decl(Vec,vec_vec_zz)

/**
 * This macro allows the use of the input and output operators
 * \f$\scriptstyle{<<}\f$ and \f$\scriptstyle{>>}\f$ for type  \c vec_vec_zz.
 */
NTL_io_vector_decl(Vec,vec_vec_zz)

/**
 * This macro allows the use of the equality operators \f$==\f$ and \f$!\!=\f$ for type
 * \c vec_vec_zz.
 */
NTL_eq_vector_decl(Vec,vec_vec_zz)

#endif
