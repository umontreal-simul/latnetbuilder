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

#ifndef LATBUILDER__KERNEL__R_ALPHA_H
#define LATBUILDER__KERNEL__R_ALPHA_H

#include "latbuilder/Kernel/Base.h"
#include "latbuilder/Types.h"
#include "latbuilder/fftw++.h"

#include <boost/math/constants/constants.hpp>

#include <sstream>
#include <cmath>

namespace LatBuilder { namespace Kernel {

/**
 * One-dimensional merit function for the \f$\mathcal R_\alpha\f$ figure of
 * merit for any \f$\alpha \geq 0\f$.
 *
 * This merit function is defined as
 * \f[
 *    \omega(x)
 *    = r_{\alpha,n}(x)
 *    = \sum_{h = -\lfloor (n-1)/2 \rfloor}^{\lfloor n/2 \rfloor}
 *      |\max(1, h)|^{-\alpha} e^{2 \pi i h x} - 1
 * \f]
 * For even \f$n\f$, the sum is over \f$h=-n/2+1,\dots,n/2\f$; for odd \f$n\f$,
 * the sum is over \f$h=-(n-1)/2,\dots,(n-1)/2\f$.
 * By replacing \f$h\f$ with \f$-h\f$ in the part of the sum that is over the
 * negative values of \f$h\f$, we obtain:
 * \f[
 *    r_{\alpha,n}(x)
 *    = 2 \sum_{h=1}^{\lfloor (n-1)/2 \rfloor} h^{-\alpha} \cos(2\pi h x)
 *    + \mathbb 1[\text{$n$ is even}] \, (n/2)^{-\alpha} e^{i\pi n x}
 * \f]
 * This is the expression implemented by #pointValue().  Note that the last term
 * is nonzero only if \f$n\f$ is even and, for \f$x=j/n\f$, is equal to
 * \f$(-1)^j\f$ for any integer \f$j\f$.
 *
 * \remark This functor only returns the real part of the last term.
 *
 * Alternatively, by replacing \f$h\f$ with \f$h-n\f$ in the part of the sum
 * that is over the negative values of \f$h\f$, we obtain:
 * \f[
 *    r_{\alpha,n}(x)
 *    = \sum_{h=0}^{n-1} \hat r_{\alpha,n}(h) e^{2 \pi i h x},
 * \f]
 * where
 * \f[
 *    \hat r_{\alpha,n}(h) =
 *    \begin{cases}
 *      0 & \text{if $h=0$} \\
 *      h^{-\alpha} & \text{if $0   < h \leq n/2$} \\
 *      (n-h)^{-\alpha} & \text{if $n/2 < h <    n  $} \\
 *    \end{cases}
 * \f]
 * The values of \f$r_{\alpha,n}(j/n)\f$ for \f$j=0,\dots,n-1\f$
 * are the \f$n\f$-point inverse discrete Fourier transform of \f$\hat
 * r_{\alpha,n}(h)\f$ at \f$h=0,\dots,n-1\f$.  This is how #valuesVector()
 * computes these values.
 *
 * \tparam LAT          Type of lattice.
 */
class RAlpha : public Base<RAlpha> {
public:
   /**
    * Constructor.
    *
    * \param alpha Value of \f$\alpha\f$.
    */
   RAlpha(Real alpha):
      m_alpha(alpha)
   {}

   /**
    * Returns the one-dimensional function evaluated at \c x.
    *
    * \param x    Point at which the function must be evaluated.
    * \param n    Number of terms in the sum (see class documentation).
    *
    * \remark Returns only the real part of the kernel value.
    */
   Real pointValue(const Real& x, Modulus n) const
   {
      const auto pi = boost::math::constants::pi<Real>();
      Modulus hmax = (n - 1) / 2;
      Real sum = 0.0;
      for (Modulus h = 1; h <= hmax; h++)
         sum += std::pow(h, -alpha()) * 2 * std::cos(2 * pi * h * x);
      if (n % 2 == 0)
         sum += std::pow(n / 2, -alpha()) * cos(pi * n * x);
      return sum;
   }

   /**
    * \copydoc Base::valuesVector()
    *
    * Creates a new vector of kernel values using fast Fourier transforms.
    */
   template <LatType L, Compress C>
   RealVector valuesVector(
         const Storage<L, C>& storage
         ) const
   {
      fftw<Real>::real_vector rvec(storage.sizeParam().numPoints());
      fftw<Real>::complex_vector cvec(storage.sizeParam().numPoints() / 2 + 1);
      cvec[0] = 0;
      for (size_t h = 1; h < cvec.size(); h++)
         cvec[h] = std::pow(h, -alpha());

      fftw<Real>::ifft(cvec, rvec, false);

      RealVector vec(storage.size());
      auto proxy = storage.unpermuted(vec);

      for (size_t i = 0; i < vec.size(); i++)
         proxy(i) = rvec[i];

      return vec;
   }

   /**
    * Returns the value of \f$\alpha\f$.
    */
   Real alpha() const
   { return m_alpha; }

   bool symmetric() const
   { return true; }

   static constexpr Compress suggestedCompression()
   { return Compress::SYMMETRIC; }

   std::string name() const
   { std::ostringstream os; os << "R" << alpha(); return os.str(); }

private:
   Real m_alpha;
};

}}

#endif
