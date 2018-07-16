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

#ifndef LATBUILDER__NORM__PALPHA_PLR_H
#define LATBUILDER__NORM__PALPHA_PLR_H

#include "latbuilder/Norm/NormAlphaBase.h"
#include "latbuilder/Types.h"
#include "latbuilder/CombinedWeights.h"

#include "latticetester/ProjectionDependentWeights.h"
#include "latticetester/OrderDependentWeights.h"
#include "latticetester/ProductWeights.h"
#include "latticetester/PODWeights.h"

namespace LatBuilder { namespace Norm {

/**
 * Bound on the weighted \f$\mathcal P_\alpha\f$PLR discrepancy.
 *
 * This is the general bound derived in Theorem 3 of cite TODO for
 * projection-dependent weights.
 * The theorem states that, for \f$\mathcal D^2(\boldsymbol a_s, n)
 * = \mathcal P_\alphaPLR(\boldsymbol a_s, n)\f$, there exists a generating vector
 * \f$\boldsymbol a_s\f$ such that
 * \f[
 *    \mathcal D^2(\boldsymbol a_s, n) \leq N_{n,s}(\lambda)
 * \f]
 * for any \f$\lambda \in (1/\alpha,1]\f$, where
 * \f[
 *    N_{n,s}(\lambda) =
 *    \left[
 *       \frac{2}{n}
 *       \sum_{\emptyset \neq \mathfrak u \subseteq \{1,\dots,s\}}
 *       \gamma_{\mathfrak u}^\lambda \,
 *       \left( \mu(\alpha\lambda) \right)^{|\mathfrak u|}
 *    \right]^{1/\lambda},
 * \f]
 * in which \f$\mu(x) = \frac{2^x}{2^x - 2} \f$.
 * The normalization that is used is:
 * \f[
 *    \min_\lambda N_{n,s}(\lambda)
 * \f]
 *
 * For order-dependent weights, the bound can be rewritten as:
 * \f[
 *    N_{n,s}(\lambda) =
 *    \left[
 *       \frac{2}{n}
 *       \sum_{\ell=1}^s \Gamma_\ell^\lambda \, \frac{s!}{\ell! (s-\ell)!}
 *       \,
 *       \left( 2 \beta(\alpha\lambda) \right)^\ell
 *    \right]^{1/\lambda}
 * \f]
 *
 * For product weights, it can be written as:
 * \f[
 *    N_{n,s}(\lambda) =
 *    \left\{
 *       \frac{2}{n}
 *       \left[
 *         \prod_{j=1}^s
 *         \left( 1 + \gamma_j^\lambda \, \mu(\alpha\lambda) \right)
 *         - 1
 *       \right]
 *    \right\}^{1/\lambda},
 * \f]
 *
 * For product and order-dependent (POD) weights, the bound can be written as:
 * \f[
 *    N_{n,s}(\lambda) =
 *    \left[
 *       \frac{2}{n}
 *       \sum_{\ell=1}^s \Gamma_\ell^\lambda \,
 *       e_\ell^d \left( \mu(\alpha\lambda) \gamma_1^\lambda, \dots, \mu(\alpha\lambda) \gamma_d^\lambda  \right)^\ell
 *    \right]^{1/\lambda}
 *  \f]
 * where \f$e_\ell^d\f$ designates the elementary symmetric polynomial of degree \f$l\f$ with \f$d\f$ variables.
 *
 */
class PAlphaPLR : public NormAlphaBase<PAlphaPLR> {
public:
   /**
    * Constructor.
    * 
    * \param alpha         Smoothness level \f$\alpha\f$ of the class of functions.
    * \param weights       Projection-dependent weights \f$ \gamma_{\mathfrak u} \f$.
    * \param normType      Type of cross-projection norm used by the figure of
    *                      merit.
    */
   PAlphaPLR(unsigned int alpha, const LatticeTester::Weights& weights, Real normType=2);

   template <LatticeType LR, EmbeddingType L>
   Real value(
         Real lambda,
         const SizeParam<LR, L>& sizeParam,
         Dimension dimension,
         Real norm = 1.0
         ) const;

   std::string name() const
   { return "PAlphaPLR"; }

private:
   const LatticeTester::Weights& m_weights;
};

}}

#endif
