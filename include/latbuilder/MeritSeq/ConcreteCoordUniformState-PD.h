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

#ifndef LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_PD_H
#define LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_PD_H

#include "latbuilder/MeritSeq/CoordUniformState.h"
#include "latbuilder/Storage.h"

#include "latcommon/ProjectionDependentWeights.h"
#include "latcommon/Coordinates.h"

#include <map>


namespace LatBuilder { namespace MeritSeq {

// forward declaration
template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO, class WEIGHTS > class ConcreteCoordUniformState;

/**
 * Implementation of CoordUniformState for projection-dependent weights.
 *
 * Define
 * \f[
 *   \boldsymbol p_{\mathfrak u} = \left(
 *      \prod_{j \in \mathfrak u} \; \omega(x_{0,j}),
 *      \dots,
 *      \prod_{j \in \mathfrak u} \; \omega(x_{n-1,j})
 *      \right)
 *    \quad (\forall \mathfrak u \subseteq \{1,\dots,s\})
 * \f]
 * for \f$s \geq 1\f$.  Then, for \f$s \geq 1\f$, we have
 * \f[
 *    \mathcal D_s^2 = \mathcal D_{s-1}^2 +
 *    \frac1n \boldsymbol\omega_s \cdot
 *    \sum_{\mathfrak u \subseteq \{1,\dots,s-1\}}
 *    \gamma_{\mathfrak u \cup \{s\}}
 *    \, \boldsymbol p_{\mathfrak u}
 * \f]
 * and
 * \f[
 *    \boldsymbol p_{\mathfrak u \cup \{s\}} =
 *       \boldsymbol\omega_s \odot \boldsymbol p_{\mathfrak u}
 *       \quad (\forall \mathfrak u \subseteq \{1,\dots,s-1\}),
 * \f]
 * where \f$\cdot\f$ and \f$\odot\f$ denote the scalar and the element-by-element
 * product, respectively.
 * For \f$s=0\f$ and \f$\mathfrak u = \emptyset\f$, we artificially define
 * \f$\mathcal D_0^2 = 0\f$ and \f$\boldsymbol p_{\emptyset} = \boldsymbol 1\f$.
 *
 * See CoordUniformCBC for the definition of
 * \f$\boldsymbol \omega_s\f$.
 */
template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
class ConcreteCoordUniformState<LR, LAT, COMPRESS, PLO, LatCommon::ProjectionDependentWeights> :
   public CoordUniformState<LR, LAT, COMPRESS, PLO> {
public:

  typedef typename LatticeTraits<LR>::GeneratingVector GeneratingVector;
   /**
    * Constructor.
    *
    * Sets \f$\mathcal D_0^2 = 0\f$ and \f$\boldsymbol p_{\emptyset} =
    * \boldsymbol 1\f$.
    * 
    * \param storage       Storage configuration.
    * \param weights       Projection-dependent weights \f$ \gamma_{\mathfrak u} \f$.
    */
   ConcreteCoordUniformState(
         const Storage<LR, LAT, COMPRESS, PLO>& storage,
         const LatCommon::ProjectionDependentWeights& weights
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
    *    \boldsymbol p_{\mathfrak u \cup \{s\}} =
    *       \boldsymbol\omega_s \odot \boldsymbol p_{\mathfrak u}
    *       \quad (\forall \mathfrak u \subseteq \{1,\dots,s-1\}).
    * \f]
    */
   void update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen);

   /**
    * \copydoc CoordUniformState::weightedState()
    *
    * Computes
    * \f[
    *    \boldsymbol q_s =
    *    \sum_{\mathfrak u \subseteq \{1,\dots,s\}}
    *    \gamma_{\mathfrak u \cup \{s+1\}}
    *    \, \boldsymbol p_{\mathfrak u}.
    * \f]
    */
   RealVector weightedState() const;

   /// \copydoc CoordUniformState::clone()
   std::unique_ptr<CoordUniformState<LR, LAT, COMPRESS, PLO>> clone() const
   { return std::unique_ptr<CoordUniformState<LR, LAT, COMPRESS, PLO>>(new ConcreteCoordUniformState(*this)); }

private:
   const LatCommon::ProjectionDependentWeights& m_weights;

   // m_state[projection](i)
   // declared mutable because it is updated transparently by #getStateVector()
   std::map<LatCommon::Coordinates, RealVector> m_state;

   // keep track of the selected generator values to be able to generate state
   // vectors on demand
   GeneratingVector m_gen;

   /**
    * Creates and stores the state vector for \c projection if it has not been
    * created yet.
    *
    * \return A reference to the state vector.
    */
   const RealVector& createStateVector(const LatCommon::Coordinates& projection, const RealVector& kernelValues);
};



extern template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::ORDINARY, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::ProjectionDependentWeights>;
extern template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::ORDINARY, Compress::SYMMETRIC, PerLvlOrder::BASIC, LatCommon::ProjectionDependentWeights>;
extern template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::CYCLIC,      LatCommon::ProjectionDependentWeights>;
extern template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::EMBEDDED, Compress::SYMMETRIC, PerLvlOrder::CYCLIC, LatCommon::ProjectionDependentWeights>;

extern template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::ORDINARY, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::ProjectionDependentWeights>;
extern template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::CYCLIC,      LatCommon::ProjectionDependentWeights>;


extern template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::ProjectionDependentWeights>;

}}

#endif
