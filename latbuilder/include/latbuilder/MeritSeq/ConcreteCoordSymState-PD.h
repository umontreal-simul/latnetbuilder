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

#ifndef LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_PD_H
#define LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_PD_H

#include "latbuilder/MeritSeq/CoordSymState.h"
#include "latbuilder/Storage.h"

#include "latcommon/ProjectionDependentWeights.h"
#include "latcommon/Coordinates.h"

#include <map>


namespace LatBuilder { namespace MeritSeq {

// forward declaration
template <LatType LAT, Compress COMPRESS, class WEIGHTS> class ConcreteCoordSymState;

/**
 * Implementation of CoordSymState for projection-dependent weights.
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
 * See CoordSymCBC for the definition of
 * \f$\boldsymbol \omega_s\f$.
 */
template <LatType LAT, Compress COMPRESS>
class ConcreteCoordSymState<LAT, COMPRESS, LatCommon::ProjectionDependentWeights> :
   public CoordSymState<LAT, COMPRESS> {
public:
   /**
    * Constructor.
    *
    * Sets \f$\mathcal D_0^2 = 0\f$ and \f$\boldsymbol p_{\emptyset} =
    * \boldsymbol 1\f$.
    * 
    * \param storage       Storage configuration.
    * \param weights       Projection-dependent weights \f$ \gamma_{\mathfrak u} \f$.
    */
   ConcreteCoordSymState(
         const Storage<LAT, COMPRESS>& storage,
         const LatCommon::ProjectionDependentWeights& weights
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
    *    \boldsymbol p_{\mathfrak u \cup \{s\}} =
    *       \boldsymbol\omega_s \odot \boldsymbol p_{\mathfrak u}
    *       \quad (\forall \mathfrak u \subseteq \{1,\dots,s-1\}).
    * \f]
    */
   void update(const RealVector& kernelValues, Modulus gen);

   /**
    * \copydoc CoordSymState::weightedState()
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

   /// \copydoc CoordSymState::clone()
   std::unique_ptr<CoordSymState<LAT, COMPRESS>> clone() const
   { return std::unique_ptr<CoordSymState<LAT, COMPRESS>>(new ConcreteCoordSymState(*this)); }

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

extern template class ConcreteCoordSymState<LatType::ORDINARY, Compress::NONE,      LatCommon::ProjectionDependentWeights>;
extern template class ConcreteCoordSymState<LatType::ORDINARY, Compress::SYMMETRIC, LatCommon::ProjectionDependentWeights>;
extern template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::NONE,      LatCommon::ProjectionDependentWeights>;
extern template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::SYMMETRIC, LatCommon::ProjectionDependentWeights>;

}}

#endif
