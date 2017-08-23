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

#ifndef LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_P_H
#define LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_P_H

#include "latbuilder/MeritSeq/CoordUniformState.h"
#include "latbuilder/Storage.h"

#include "latcommon/ProductWeights.h"

#include <memory>

namespace LatBuilder { namespace MeritSeq {

// forward declaration
template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO, class WEIGHTS> class ConcreteCoordUniformState;

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
template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO >
class ConcreteCoordUniformState<LR, LAT, COMPRESS, PLO, LatCommon::ProductWeights> :
   public CoordUniformState<LR, LAT, COMPRESS, PLO> {
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
         const Storage<LR, LAT, COMPRESS, PLO>& storage,
         const LatCommon::ProductWeights& weights
         ):
      CoordUniformState<LR, LAT, COMPRESS, PLO>(storage),
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
   void update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen);

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
   std::unique_ptr<CoordUniformState<LR, LAT, COMPRESS, PLO>> clone() const
   { return std::unique_ptr<CoordUniformState<LR, LAT, COMPRESS, PLO>>(new ConcreteCoordUniformState(*this)); }

private:
   const LatCommon::ProductWeights& m_weights;

   // m_state(i)
   RealVector m_state;
};


extern template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::ORDINARY, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::ProductWeights>;
extern template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::ORDINARY, Compress::SYMMETRIC, PerLvlOrder::BASIC, LatCommon::ProductWeights>;
extern template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::CYCLIC,      LatCommon::ProductWeights>;
extern template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::EMBEDDED, Compress::SYMMETRIC, PerLvlOrder::CYCLIC, LatCommon::ProductWeights>;

extern template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::ORDINARY, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::ProductWeights>;
extern template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::CYCLIC,      LatCommon::ProductWeights>;


extern template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::ProductWeights>;
}}

#endif
