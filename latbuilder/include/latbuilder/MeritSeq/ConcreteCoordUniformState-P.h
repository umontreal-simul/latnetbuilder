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

#ifndef LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_P_H
#define LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_P_H

#include "latbuilder/MeritSeq/CoordUniformState.h"
#include "latbuilder/Storage.h"

#include "latcommon/ProductWeights.h"

#include <memory>

namespace LatBuilder { namespace MeritSeq {

// forward declaration
template <LatType LAT, Compress COMPRESS, class WEIGHTS> class ConcreteCoordUniformState;

/**
 * Implementation of CoordUniformState for product weights.
 *
 * This formulation is as proposed in \cite rCOO06a .
 * Define
 * \f[
 *   \boldsymbol p_s = \left(
 *      \prod_{j=1}^s
 *      \left[ 1 + \gamma_j \omega(x_{0,j}) \right],
 *      \dots,
 *      \prod_{j=1}^s
 *      \left[ 1 + \gamma_j \omega(x_{n-1,j}) \right]
 *      \right),
 * \f]
 * for \f$s \geq 1\f$, with \f$\boldsymbol p_0 = \boldsymbol 1\f$.
 * Then,
 * \f[
 *    \mathcal D_s^2 = \mathcal D_{s-1}^2 +
 *    \frac{\gamma_s}{n} \boldsymbol\omega_s \cdot \boldsymbol p_{s-1}
 * \f]
 * and
 * \f[
 *    \boldsymbol p_s =
 *       (\boldsymbol 1 + \gamma_s \boldsymbol\omega_s) \odot \boldsymbol p_{s-1}.
 * \f]
 */
template <LatType LAT, Compress COMPRESS>
class ConcreteCoordUniformState<LAT, COMPRESS, LatCommon::ProductWeights> :
   public CoordUniformState<LAT, COMPRESS> {
public:
   /**
    * Constructor.
    *
    * Sets \f$\mathcal D_0^2 = 0\f$ and \f$\boldsymbol p_0 = \boldsymbol
    * 1\f$.
    * 
    * \param storage       Storage configuration.
    * \param weights       Product weights \f$ \gamma_{\mathfrak u} \f$.
    */
   ConcreteCoordUniformState(
         const Storage<LAT, COMPRESS>& storage,
         const LatCommon::ProductWeights& weights
         ):
      CoordUniformState<LAT, COMPRESS>(storage),
      m_weights(weights)
   { reset(); }

   void reset();

   /**
    * \copydoc CoordUniformState::update()
    *
    * Computes
    * \f[
    *    \boldsymbol p_s =
    *       (\boldsymbol 1 + \gamma_s \boldsymbol\omega_s) \odot \boldsymbol p_{s-1}.
    * \f]
    */
   void update(const RealVector& kernelValues, Modulus gen);

   /**
    * Computes the weighted combination state vectors.
    *
    * Computes
    * \f[
    *    \boldsymbol q_s = \gamma_{s+1} \boldsymbol p_s.
    * \f]
    */
   RealVector weightedState() const;

   /// \copydoc CoordUniformState::clone()
   std::unique_ptr<CoordUniformState<LAT, COMPRESS>> clone() const
   { return std::unique_ptr<CoordUniformState<LAT, COMPRESS>>(new ConcreteCoordUniformState(*this)); }

private:
   const LatCommon::ProductWeights& m_weights;

   // m_state(i)
   RealVector m_state;
};

extern template class ConcreteCoordUniformState<LatType::ORDINARY, Compress::NONE,      LatCommon::ProductWeights>;
extern template class ConcreteCoordUniformState<LatType::ORDINARY, Compress::SYMMETRIC, LatCommon::ProductWeights>;
extern template class ConcreteCoordUniformState<LatType::EMBEDDED, Compress::NONE,      LatCommon::ProductWeights>;
extern template class ConcreteCoordUniformState<LatType::EMBEDDED, Compress::SYMMETRIC, LatCommon::ProductWeights>;

}}

#endif
