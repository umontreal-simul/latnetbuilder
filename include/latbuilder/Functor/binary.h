// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#ifndef LATBUILDER__FUNCTOR__BINARY_H
#define LATBUILDER__FUNCTOR__BINARY_H

#include <string>
#include <algorithm>
#include <cmath>
#include "latbuilder/Types.h"
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
   { return "Sum"; }
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
   { return "Max"; }
};

/**
 * Raise to power.
 */
struct Pow {
   typedef Real result_type;
   typedef Real arg_type;

   static result_type apply(const arg_type& x, const arg_type& y)
   { return std::pow(x, y); }

   static std::string name()
   { return "Pow"; }
};

}}

#endif
