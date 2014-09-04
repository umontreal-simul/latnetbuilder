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
  * This class builds matrices of \c double's and is very similar to some NTL
  * classes for matrices of reals. It has been added for compatibility with NTL in
  * declaration of types and method calls. The macros used provide template-like
  * classes for dynamic-sized arrays of \c double's.
  * 
  * 
  * 
 * 
  */
 
#ifndef MAT_DOUBLE_H
#define MAT_DOUBLE_H

#include "NTL/matrix.h"
#include "latcommon/vec_double.h"
#include "latcommon/vec_vec_double.h"


typedef vec_vec_double VecDouble_VecDouble; 

NTL_matrix_decl(Double,VecDouble,VecDouble_VecDouble,mat_double)


#endif

