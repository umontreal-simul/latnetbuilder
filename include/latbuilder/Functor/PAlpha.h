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
