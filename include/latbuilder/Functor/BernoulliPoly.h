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
