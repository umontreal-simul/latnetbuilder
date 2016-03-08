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

   template <LatType L>
   Real value(
         Real lambda,
         const SizeParam<L>& sizeParam,
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
