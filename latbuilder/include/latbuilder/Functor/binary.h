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

#ifndef LATBUILDER__FUNCTOR__BINARY_H
#define LATBUILDER__FUNCTOR__BINARY_H

#include <string>
#include <algorithm>

namespace LatBuilder { namespace Functor {

/**
 * Scalar sum binary operator.
 */
template <typename T>
struct Sum {
   typedef T result_type;
   typedef T arg_type;

   static result_type apply(const arg_type& x, const arg_type& y)
   { return x + y; }

   static std::string name()
   { return "sum"; }
};

/**
 * Scalar max binary operator.
 */
template <typename T>
struct Max {
   typedef T result_type;
   typedef T arg_type;

   static result_type apply(const arg_type& x, const arg_type& y)
   { return std::max(x, y); }

   static std::string name()
   { return "max"; }
};

}}

#endif
