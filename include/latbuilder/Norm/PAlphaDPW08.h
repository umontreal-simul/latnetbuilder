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

#ifndef LATBUILDER__NORM__PALPHA_DICK08_H
#define LATBUILDER__NORM__PALPHA_DICK08_H

#include "latbuilder/Norm/PAlphaBase.h"
#include "latbuilder/Types.h"

#include "latcommon/ProductWeights.h"
#include "latbuilder/CombinedWeights.h"

namespace LatBuilder { namespace Norm {

/**
 * Bound on the average weighted \f$\mathcal P_\alpha\f$ discrepancy.
 *
 * This is the bound
 * \f[
 *   \min_{1/\alpha < \lambda \leq 1} \overline{E}^2_{n,s,\alpha,\boldsymbol\gamma}(\lambda)
 * \f]
 * from Theorem 10 (with the minimization step proposed in Algorithm 2) in \cite vDIC08c .
 */
class PAlphaDPW08 : public PAlphaBase<PAlphaDPW08> {
public:
   /**
    * Constructor.
    * 
    * \param alpha         Smoothness level \f$\alpha\f$ of the class of functions.
    * \param weights       Projection-dependent weights \f$ \gamma_{\mathfrak u} \f$.
    * \param normType      Type of cross-projection norm used by the figure of
    *                      merit.
    */
   PAlphaDPW08(unsigned int alpha, const LatCommon::Weights& weights, Real normType);

   template <Lattice LR, LatType L>
   Real value(
         Real lambda,
         const SizeParam<LR, L>& sizeParam,
         Dimension dimension,
         Real norm = 1.0
         ) const;

   std::string name() const
   { return "PAlphaDPW08"; }

private:
   const LatCommon::Weights& m_weights;
};

}}

#endif
