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

#ifndef LATBUILDER__NORM__PALPHA_SL10_H
#define LATBUILDER__NORM__PALPHA_SL10_H

#include "latbuilder/Norm/PAlphaBase.h"
#include "latbuilder/Types.h"
#include "latbuilder/CombinedWeights.h"

#include "latcommon/ProjectionDependentWeights.h"
#include "latcommon/OrderDependentWeights.h"
#include "latcommon/ProductWeights.h"
#include "latcommon/PODWeights.h"

namespace LatBuilder { namespace Norm {

/**
 * Bound on the weighted \f$\mathcal P_\alpha\f$ discrepancy.
 *
 * This is the general bound derived in Theorem 3 of \cite rSIN11a for
 * projection-dependent weights.
 * The theorem states that, for \f$\mathcal D^2(\boldsymbol a_s, n)
 * = \mathcal P_\alpha(\boldsymbol a_s, n)\f$, there exists a generating vector
 * \f$\boldsymbol a_s \in \mathbb Z^s\f$ such that
 * \f[
 *    \mathcal D^2(\boldsymbol a_s, n) \leq N_{n,s}(c, \lambda)
 * \f]
 * for any \f$\lambda \in (1/\alpha,1]\f$ and any \f$c \in [0, 1]\f$, where
 * \f[
 *    N_{n,s}(c, \lambda) =
 *    \left[
 *       \frac{1}{c \, \varphi(n)}
 *       \sum_{\emptyset \neq \mathfrak u \subseteq \{1,\dots,s\}}
 *       \gamma_{\mathfrak u}^\lambda \,
 *       \left( 2 \zeta(\alpha\lambda) \right)^{|\mathfrak u|}
 *    \right]^{1/\lambda},
 * \f]
 * in which \f$\zeta\f$ is the Riemann zeta function and \f$\varphi\f$ is
 * Euler's totient function.
 * The normalization that is used is:
 * \f[
 *    \min_\lambda N_{n,s}(c, \lambda)
 * \f]
 *
 * For order-dependent weights, the bound can be rewritten as:
 * \f[
 *    N_{n,s}(c, \lambda) =
 *    \left[
 *       \frac{1}{c \, \varphi(n)}
 *       \sum_{\ell=1}^s \Gamma_\ell^\lambda \, \frac{s!}{\ell! (s-\ell)!}
 *       \,
 *       \left( 2 \zeta(\alpha\lambda) \right)^\ell
 *    \right]^{1/\lambda}
 *    =
 *    \left[
 *       \frac{1}{c \, \varphi(n)}
 *       \sum_{\ell=1}^s \Gamma_\ell^\lambda \, y_\ell(\lambda)
 *    \right]^{1/\lambda},
 * \f]
 * where
 * \f[
 *    y_\ell(\lambda) = \frac{s - \ell + 1}{\ell}
 *    \, 2 \zeta(\alpha\lambda)
 *    \times y_{\ell - 1}(\lambda)
 * \f]
 * for \f$\ell \geq 1\f$ and \f$y_0(\alpha) = 1\f$.
 *
 *
 * For product weights, it can be written as:
 * \f[
 *    N_{n,s}(c, \lambda) =
 *    \left\{
 *       \frac{1}{c \, \varphi(n)}
 *       \left[
 *         \prod_{j=1}^s
 *         \left( 1 + 2 \gamma_j^\lambda \, \zeta(\alpha\lambda) \right)
 *         - 1
 *       \right]
 *    \right\}^{1/\lambda},
 * \f]
 *
 * For product and order-dependent (POD) weights, the bound can be written as:
 * \f[
 *    N_{n,s}(c, \lambda) =
 *    \left[
 *       \frac{1}{c \, \varphi(n)}
 *       \sum_{\ell=1}^s \Gamma_\ell^\lambda \, y_\ell(\lambda)
 *    \right]^{1/\lambda},
 * \f]
 * where
 * \f[
 *    y_\ell(\lambda) = \frac{s - \ell + 1}{\ell}
 *    \, 2 \gamma_\ell^\lambda \zeta(\alpha\lambda)
 *    \times y_{\ell - 1}(\lambda)
 * \f]
 * for \f$\ell \geq 1\f$ and \f$y_0(\lambda) = 1\f$.
 *
 */
class PAlphaSL10 : public PAlphaBase<PAlphaSL10> {
public:
   /**
    * Constructor.
    * 
    * \param alpha         Smoothness level \f$\alpha\f$ of the class of functions.
    * \param weights       Projection-dependent weights \f$ \gamma_{\mathfrak u} \f$.
    * \param normType      Type of cross-projection norm used by the figure of
    *                      merit.
    */
   PAlphaSL10(unsigned int alpha, const LatCommon::Weights& weights, Real normType=2);

   template <LatType L>
   Real value(
         Real lambda,
         const SizeParam<L>& sizeParam,
         Dimension dimension,
         Real norm = 1.0
         ) const;

   std::string name() const
   { return "PAlphaSL10"; }

private:
   const LatCommon::Weights& m_weights;
};

}}

#endif
