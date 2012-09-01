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

#include "latcommon/vec_double.h"
#include <cstdio>


//NTL_START_IMPL

//namespace latMRG
//{

NTL_vector_impl(Double, vec_double)

NTL_io_vector_impl(Double, vec_double)

NTL_eq_vector_impl(Double, vec_double)


//}

//NTL_END_IMPL
