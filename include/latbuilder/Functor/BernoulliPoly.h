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

#ifndef LATBUILDER__FUNCTOR__BERNOULLI_POLY_H
#define LATBUILDER__FUNCTOR__BERNOULLI_POLY_H

#include "latbuilder/Types.h"

namespace LatBuilder { namespace Functor {

/**
 * Bernoulli polynomial.
 *
 * The first few Bernoulli polynomials:
 * \f{eqnarray*}{
 *    B_0(x) &=& 1 \\
 *    B_1(x) &=& x - 1/2 \\
 *    B_2(x) &=& x^2 - x + 1/6  \\
 *    B_3(x) &=& x^3 - 3x^2/2 + x/2  \\
 *    B_4(x) &=& x^4 - 2x^3 + x^2 - 1/30 \\
 *    B_5(x) &=& x^5 - 5x^4/2 + 5x^3/3 - x/6 \\
 *    B_6(x) &=& x^6 - 3x^5 + 5x^4/2 - x^2/2 + 1/42 \\
 *    B_8(x) &=& x^8 - 4x^7 + 14x^6/3 - 7x^4/3 + 2x^2/3 - 1/30.
 * \f}
 * where \f$B_{2\alpha}(x)\f$ is the Bernoulli polynomial of degree \f$2\alpha\f$.
 *
 * \tparam DEGREE    Degree of the polynomial.
 *
 * \remark Specializations are implemented up to degree 8.
 */
template <unsigned int DEGREE>
struct BernoulliPoly;

#define DEFINE_BERNOULLI_POLY(deg, poly) \
   template<> \
   struct BernoulliPoly<deg> { \
   public: \
      typedef Real value_type; \
      typedef Real result_type; \
      inline static constexpr unsigned int degree() { return (deg); } \
      inline static constexpr bool symmetric() { return true; } \
      inline static std::string name() { return "Bernoulli polynomial of degree " #deg ""; } \
      inline static result_type apply(const value_type& x) { return (poly); } \
   }

DEFINE_BERNOULLI_POLY(0, 1.0);
DEFINE_BERNOULLI_POLY(1, x - 0.5);
DEFINE_BERNOULLI_POLY(2, x * (x - 1.0) + (1.0 / 6.0));
DEFINE_BERNOULLI_POLY(3, ((2.0 * x - 3.0) * x + 1.0) * x * 0.5);
DEFINE_BERNOULLI_POLY(4, ((x - 2.0) * x + 1.0) * x * x - (1.0 / 30.0));
DEFINE_BERNOULLI_POLY(5, (((x - 2.5) * x + (5.0 / 3.0)) * x * x - (1.0 / 6.0)) * x);
DEFINE_BERNOULLI_POLY(6, (((x - 3.0) * x + 2.5) * x * x - 0.5) * x * x + (1.0 / 42.0));
DEFINE_BERNOULLI_POLY(7, ((((x - 3.5) * x + 3.5) * x * x - 7.0/6.0) * x * x + (1.0 / 6.0)) * x);
DEFINE_BERNOULLI_POLY(8, ((((x - 4.0) * x + (14.0 / 3.0)) * x * x - (7.0 / 3.0)) * x * x + (2.0 / 3.0)) * x * x - (1.0 / 30.0));

#undef DEFINE_BERNOULLI_POLY

}}

#endif
