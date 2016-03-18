// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
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

#ifndef LATBUILDER__FUNCTOR__PALPHA_H
#define LATBUILDER__FUNCTOR__PALPHA_H

#include "latbuilder/Functor/BernoulliPoly.h"
#include "latbuilder/Types.h"

#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/factorials.hpp>

#include <cmath>
#include <sstream>

namespace LatBuilder { namespace Functor {

/**
 * One-dimensional merit function for the \f$\mathcal P_\alpha\f$ discrepancy.
 *
 * This merit function is defined as
 * \f[
 *    \omega(x) =
 *    -\frac{(-4\pi^2)^{\alpha/2}}{\alpha!} \, B_\alpha(x),
 * \f]
 * for even integers \f$\alpha\f$, where \f$B_\alpha(x)\f$ is the Bernoulli
 * polynomial of degree \f$\alpha\f$.
 */
class PAlpha {
public:
   typedef Real value_type;
   typedef Real result_type;

   /**
    * Constructor.
    *
    * \param alpha     Value of \f$\alpha\f$.
    */
   PAlpha(unsigned int alpha):
      m_alpha(alpha)
   {
      if (m_alpha == 2)
         m_bernoulli = BernoulliPoly<2>::apply;
      else if (m_alpha == 4)
         m_bernoulli = BernoulliPoly<4>::apply;
      else if (m_alpha == 6)
         m_bernoulli = BernoulliPoly<6>::apply;
      else if (m_alpha == 8)
         m_bernoulli = BernoulliPoly<8>::apply;
      else
         throw std::runtime_error("PAlpha: alpha must be 2, 4, 6 or 8");

      m_scaling = -imagPow(m_alpha) * Real(
            std::pow(2 * pi(), m_alpha) / fac(m_alpha)
            );
   }

   unsigned int alpha() const
   { return m_alpha; }

   bool symmetric() const
   { return true; }

   static constexpr Compress suggestedCompression()
   { return Compress::SYMMETRIC; }

   /**
    * Returns the one-dimensional function evaluated at \c x.
    */
   result_type operator()(const value_type& x, Modulus n = 0) const
   { return m_scaling * m_bernoulli(x); }

   std::string name() const
   { std::ostringstream os; os << "P" << alpha(); return os.str(); }

private:
   unsigned int m_alpha;
   // scaling = -((\alpha/2) % 2 ? -1 : 1) ((4 \pi^2)^(\alpha/2)) / \alpha!
   Real m_scaling;
   std::function<Real(Real)> m_bernoulli;

   static Real pi() { return boost::math::constants::pi<Real>(); }
   static Real fac(unsigned int n) { return boost::math::factorial<Real>(n); }
   static Real imagPow(unsigned int n)
   {
      if (n % 2 != 0)
         throw std::runtime_error("imagPow: argument must be even");
      return (n / 2) % 2 == 0 ? +1 : -1;
   }
};

/**
 * Formats \c functor and outputs it on \c os.
 */
inline
std::ostream& operator<<(std::ostream& os, const PAlpha& functor)
{ return os << functor.name(); }

}}

#endif
