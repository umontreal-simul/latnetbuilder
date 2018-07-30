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

#ifndef LATBUILDER__NORM__IAALPHA_H
#define LATBUILDER__NORM__IAALPHA_H

#include "latbuilder/Norm/NormAlphaBase.h"
#include "latbuilder/Types.h"
#include "latbuilder/CombinedWeights.h"

#include "latticetester/ProjectionDependentWeights.h"
#include "latticetester/OrderDependentWeights.h"
#include "latticetester/ProductWeights.h"
#include "latticetester/PODWeights.h"

namespace LatBuilder { namespace Norm {

/**
 * Bound on the interlaced \f$B_{\alpha, d,  \gamma, (1)}\f$ discrepancy.
 * 
 *
 * This is the bound 
 * \f[
 *   B_{\alpha, d,  \gamma, (1)}(\boldsymbol q_tau, p) \leq
 *   \frac{1}{(2^m - 1)^{1/\lambda}} 
 *   \left[
 *      \sum_{\emptyset \neq \mathfrak v \subseteq \{1, \dots, j_0 - 1 \}} 
 *         \tilde{\gamma}_{\mathfrak v}^\lambda G_{\alpha, d, \lambda, d, (1)}^{|\mathfrak v|} +
 *      G_{\alpha, d, \lambda, d, (1)} 
 *         \sum_{\mathfrak v \subseteq \{1, \dots, j_0 - 1 \}} \tilde{\gamma}_{\mathfrak v \cup \{j_0\}}^\lambda G_{\alpha, d, \lambda, d, (1)}^{|\mathfrak v|}
 *   \right]^{1/\lambda}.
 * \f]
 * for \f$1/\min(\alpha, d) < \lambda \leq 1\f$, where we write \f$j_0 = \lceil \tau / d \rceil\f$, \f$d_0 = \tau - (j_0 - 1)d\f$
 * and 
 * \f[
 *    G_{\alpha, d, \lambda, a, (1)} = - 1 + (1 + \tilde{G}_{\alpha, d, \lambda, (1)})^a,
 * \f]
 * for \f$a = 1, \dots, d\f$, in which we define:
 * \f[
 *    \tilde{G}_{\alpha, d, \lambda, (1)}) = \frac{1}{2^{\alpha \lambda /2}} \max \left\{ \left(\frac{1}{2^{\lambda \min(\alpha, d)} - 2}\right)^{\lambda}, \frac{1}{2^{\lambda \min(\alpha, d)} - 2} \right\}.
 * \f]
 * from Theorem 4. in \cite rGOD13a (with the minimization on \f$\lambda\f$ step proposed in Algorithm 2 in \cite vDIC08c).
 * 
 * If the \f$\gamma\f$ are POD weights, using the order-dependent correction \f$\tilde{\gamma}\f$ from Theorem 2.,
 * the computation of this bound boils down to:
 * \f[
 *    G_{\alpha, d, \lambda, d_0, (1)} (\gamma_{j_0} \Gamma_1)^\lambda + 
 *    \sum_{l = 1}^{j_0-1} \left(\Gamma_l^\lambda + G_{\alpha, d, \lambda, d_0, (1)} (\gamma_{j_0} \Gamma_{l + 1})^\lambda 
 *    e_l^{j_0-1}(G_{\alpha, d, \lambda, d, (1)} \gamma_1^\lambda, \dots, G_{\alpha, d, \lambda, d, (1)} \gamma_{j_0-1}^\lambda\right),
 * \f]
 * where \f$e_{i}^n\f$ denotes the symmetric elementary polynomial of degree \f$i\f$ with \f$n\f$ variables.
 */
class IAAlpha : public NormAlphaBase<IAAlpha> {
public:
   /**
    * Constructor.
    * 
    * \param alpha         Smoothness level \f$\alpha\f$ of the class of functions.
    * \param weights       Projection-dependent weights \f$ \gamma_{\mathfrak u} \f$.
    * \param normType      Type of cross-projection norm used by the figure of
    *                      merit.
    */
   IAAlpha(unsigned int alpha, const LatticeTester::Weights& weights, Real normType=2);

   template <LatticeType LR, EmbeddingType L>
   Real value(
         Real lambda,
         const SizeParam<LR, L>& sizeParam,
         Dimension dimension,
         Real norm = 1.0
         ) const;

   std::string name() const
   { return "IAAlpha"; }

private:
   const LatticeTester::Weights& m_weights;
};

}}

#endif
