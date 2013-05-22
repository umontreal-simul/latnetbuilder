// Copyright (c) 2012, 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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

#ifndef LATBUILDER__NORM__PALPHA_BASE_H
#define LATBUILDER__NORM__PALPHA_BASE_H

#include "latbuilder/Types.h"
#include "latbuilder/SizeParam.h"

#include <boost/math/tools/minima.hpp>

#include <limits>
#include <functional>

namespace LatBuilder { namespace Norm {

/**
 * Base class for bounds on the \f$\mathcal P_\alpha\f$ values.
 */
template <class DERIVED>
class PAlphaBase
{
public:
   /// Maximum number of iterations to be used with the minimizer.
   /// \todo Make this an instance member.
   static const unsigned MINIMIZER_MAX_ITER = 1000u;
   /// Relative precision on the minimum value to be used with the minimizer.
   /// \todo Make this an instance member.
   static const int      MINIMIZER_PREC_BITS = 18;

   /**
    * Constructor.
    * 
    * \param alpha         Smoothness level \f$\alpha\f$ of the class of functions.
    * \parm normType       Type of cross-projection norm used by the figure of
    *                      merit.
    */
   PAlphaBase(unsigned int alpha, Real normType):
      m_alpha(alpha), m_normType(normType)
   {
      // bounds for minimization domain
      m_minExp = 1.0 / m_alpha * (1.0 + std::numeric_limits<Real>::epsilon());
      m_maxExp = 1.0;
   }

   unsigned alpha() const
   { return m_alpha; }

   Real normType() const
   { return m_normType; }

   Real minExp() const
   { return m_minExp; }

   Real maxExp() const
   { return m_maxExp; }

   /**
    * Returns the value of the bound.
    *
    * \param lambda     Value of \f$\lambda = \f$.
    * \param sizeParam  Size parameter for the lattices.
    * \param dimension  Dimension.
    * \param norm       Additional normalization \f$ c \f$.
    */
   template <LatType L>
   Real value(
         Real lambda,
         const SizeParam<L>& sizeParam,
         Dimension dimension,
         Real norm = 1.0
         ) const
   { return derived().value(lambda, sizeParam, dimension, norm); }

   /**
    * Returns the smallest value of the bound for dimension \c dimension.
    *
    * \param sizeParam  Size parameter for the lattices.
    * \param dimension  Dimension.
    * \param norm       Additional normalization \f$ c \f$.
    */
   template <LatType L>
   Real operator()(
         const SizeParam<L>& sizeParam,
         Dimension dimension,
         Real norm = 1.0
         ) const
   { return minimum(sizeParam, dimension, norm); }

   /**
    * Returns the minimum value of the bound function.
    * Uses BrentMinimizer to minimize the bound function.  The value of
    * \f$\lambda\f$ at which the minimum is reached is not returned.
    */
   template <LatType L>
   Real minimum(
         const SizeParam<L>& sizeParam,
         Dimension dimension,
         Real norm
         ) const
   {
      boost::uintmax_t iter = MINIMIZER_MAX_ITER;

      std::pair<Real, Real> result = boost::math::tools::brent_find_minima(
            std::bind(&PAlphaBase::value<L>, this,
               std::placeholders::_1, sizeParam, dimension, norm),
            minExp(),
            maxExp(),
            MINIMIZER_PREC_BITS,
            iter);

      return result.second;
   }

private:
   const unsigned m_alpha;
   Real m_normType;

   Real m_minExp;
   Real m_maxExp;

   DERIVED& derived()
   { return static_cast<const DERIVED&>(*this); }
   
   const DERIVED& derived() const
   { return static_cast<const DERIVED&>(*this); }
};

}}

#endif
