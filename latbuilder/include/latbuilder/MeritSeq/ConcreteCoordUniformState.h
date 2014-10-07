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

#ifndef LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_H
#define LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_H

namespace LatBuilder { namespace MeritSeq {

/**
 * Default implementation of CoordUniformState that works with any implementation of
 * LatCommon::Weights.
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
template <LatType LAT, Compress COMPRESS, class WEIGHTS>
class ConcreteCoordUniformState :
   public CoordUniformState<LAT, COMPRESS> {
public:
   /**
    * Constructor.
    *
    * Sets \f$\mathcal D_0^2 = 0\f$ and \f$\boldsymbol p_{\emptyset} =
    * \boldsymbol 1\f$.
    * 
    * \param storage       Storage configuration.
    * \param weights       Product weights \f$ \gamma_{\mathfrak u} \f$.
    */
   ConcreteCoordUniformState(
         Storage<LAT, COMPRESS> storage,
         const WEIGHTS& weights
         ):
      CoordUniformState<LAT, COMPRESS>(std::move(storage)),
      m_weights(weights)
   { reset(); }

   void reset()
   {
	  CoordUniformState<LAT, COMPRESS>::reset();
	  m_state.clear();
	  // empty set
	  m_state[LatCommon::Coordinates()] =
		 boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 1.0);
   }

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
   void update(const RealVector& kernelValues, Modulus gen)
   {
      CoordUniformState<LAT, COMPRESS>::update(kernelValues, gen);

      using LatCommon::Coordinates;

      auto stridedKernelValues = this->storage().strided(kernelValues, gen);

      const auto newCoordinate = this->dimension() - 1;

      // iterate over all known projections
      for (auto& projState : m_state) {

         // skip (just inserted) projections that already contain the new coordinate
         if (projState.first.find(newCoordinate) != projState.first.end())
            continue;

         // create new projection
         Coordinates newProj = projState.first;
         newProj.insert(newCoordinate);

         // compute merit value for new projection
         m_state[newProj] = boost::numeric::ublas::element_prod(
               stridedKernelValues,
               projState.second
               );
      }
   }

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
   RealVector weightedState() const
   {
	  std::cerr << "warning: using default implementation of coordinate-uniform state" << std::endl;

	  using LatCommon::Coordinates;

	  const auto nextCoordinate = this->dimension();

	  RealVector weightedState =
		 boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 0.0);

	  for (auto& projState : m_state) {

		 // create new projection
		 Coordinates proj = projState.first;
		 proj.insert(nextCoordinate);

		 const Real weight = m_weights.getWeight(proj);

		 weightedState += weight * projState.second;
	  }

	  return weightedState;
   }

   std::unique_ptr<CoordUniformState<LAT, COMPRESS>> clone() const
   { return std::unique_ptr<CoordUniformState<LAT, COMPRESS>>(new ConcreteCoordUniformState(*this)); }

private:
   const WEIGHTS& m_weights;

   // m_state[projection](i)
   std::map<LatCommon::Coordinates, RealVector> m_state;
};


}}

#include "latbuilder/MeritSeq/ConcreteCoordUniformState-PD.h"
#include "latbuilder/MeritSeq/ConcreteCoordUniformState-OD.h"
#include "latbuilder/MeritSeq/ConcreteCoordUniformState-P.h"
#include "latbuilder/MeritSeq/ConcreteCoordUniformState-POD.h"

#endif
