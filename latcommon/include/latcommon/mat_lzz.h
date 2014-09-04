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
  * 
  * This class builds matrices of \c long's and is very similar to some NTL
  * classes for matrices of integers. It has been added for compatibility with NTL in
  * declaration of types and method calls. The macros used provide template-like
  * classes for dynamic-sized arrays of \c long's.
  * 
  * 
  * 
 * 
  */
 
#ifndef MAT_LZZ_H
#define MAT_LZZ_H

#include "NTL/matrix.h"
#include "latcommon/vec_lzz.h"
#include "latcommon/vec_vec_lzz.h"


typedef vec_vec_zz Vec_Vec; 

NTL_matrix_decl(Long,Vec,Vec_Vec,mat_zz)

typedef mat_zz mat_long; 


#endif

