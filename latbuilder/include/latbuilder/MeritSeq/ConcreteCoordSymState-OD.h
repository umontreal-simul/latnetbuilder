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

#ifndef LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_OD_H
#define LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_OD_H

#include "latbuilder/MeritSeq/CoordSymState.h"
#include "latbuilder/Storage.h"

#include "latcommon/OrderDependentWeights.h"

#include <vector>

namespace LatBuilder { namespace MeritSeq {

// forward declaration
template <LatType LAT, Compress COMPRESS, class WEIGHTS> class ConcreteCoordSymState;


/**
 * Implementation of CoordSymState for order-dependent weights.
 *
 * This formulation is as proposed in \cite rCOO06a.
 * Define
 * \f[
 *   \boldsymbol p_{s,\ell} = \left(
 *      \sum_{\substack{\mathfrak u \subseteq \{1, \dots, s\} \\ |\mathfrak u| = \ell}}
 *      \prod_{j \in \mathfrak u} \omega(x_{0,j}),
 *      \dots,
 *      \sum_{\substack{\mathfrak u \subseteq \{1, \dots, s\} \\ |\mathfrak u| = \ell}}
 *      \prod_{j \in \mathfrak u} \omega(x_{n-1,j})
 *      \right),
 * \f]
 * for \f$s \geq 1\f$ and \f$1 \leq \ell \leq s\f$, with \f$\boldsymbol
 * p_{0,\ell} = \boldsymbol 1\f$ and \f$\boldsymbol p_{s,s+1} = \boldsymbol
 * 0\f$.
 * Then,
 * \f[
 *    \mathcal D_s^2 = \mathcal D_{s-1}^2 +
 *    \frac1n \boldsymbol\omega_s \cdot
 *    \sum_{\ell=1}^s \Gamma_\ell \boldsymbol p_{s-1,\ell-1}
 * \f]
 * and
 * \f[
 *    \boldsymbol p_{s,\ell} =
 *       \boldsymbol p_{s-1,\ell} + \boldsymbol\omega_s \odot \boldsymbol p_{s-1,\ell-1}.
 * \f]
 */
template <LatType LAT, Compress COMPRESS>
class ConcreteCoordSymState<LAT, COMPRESS, LatCommon::OrderDependentWeights> :
   public CoordSymState<LAT, COMPRESS> {
public:
   /**
    * Constructor.
    * 
    * Sets \f$\mathcal D_0^2 = 0\f$ and \f$\boldsymbol p_{0,\ell} =
    * \boldsymbol 1\f$.
    *
    * \param storage       Storage configuration.
    * \param weights       Order-dependent weights \f$ \Gamma_\ell \f$.
    */
   ConcreteCoordSymState(
         const Storage<LAT, COMPRESS>& storage,
         const LatCommon::OrderDependentWeights& weights
         ):
      CoordSymState<LAT, COMPRESS>(storage),
      m_weights(weights)
   { reset(); }

   void reset();

   /**
    * \copydoc CoordSymState::update()
    *
    * Computes
    * \f[
    *    \boldsymbol p_{s,\ell} =
    *       \boldsymbol p_{s-1,\ell} + \boldsymbol\omega_s \odot \boldsymbol p_{s-1,\ell-1}.
    * \f]
    */
   void update(const RealVector& kernelValues, Modulus gen);

   /**
    * \copydoc CoordSymState::weightedState()
    *
    * Computes
    * \f[
    *    \boldsymbol q_s = \sum_{\ell=0}^s \Gamma_{\ell+1} \boldsymbol p_{s,\ell}.
    * \f]
    */
   RealVector weightedState() const;

   /// \copydoc CoordSymState::clone()
   std::unique_ptr<CoordSymState<LAT, COMPRESS>> clone() const
   { return std::unique_ptr<CoordSymState<LAT, COMPRESS>>(new ConcreteCoordSymState(*this)); }

private:
   const LatCommon::OrderDependentWeights& m_weights;

   // m_state[level](i)
   std::vector<RealVector> m_state;
};

extern template class ConcreteCoordSymState<LatType::ORDINARY, Compress::NONE,      LatCommon::OrderDependentWeights>;
extern template class ConcreteCoordSymState<LatType::ORDINARY, Compress::SYMMETRIC, LatCommon::OrderDependentWeights>;
extern template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::NONE,      LatCommon::OrderDependentWeights>;
extern template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::SYMMETRIC, LatCommon::OrderDependentWeights>;

}}

#endif
