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

#ifndef LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_H
#define LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_H

namespace LatBuilder { namespace MeritSeq {

/**
 * Default implementation of CoordUniformState that works with any implementation of
 * LatticeTester::Weights.
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
template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO , class WEIGHTS>
class ConcreteCoordUniformState :
   public CoordUniformState<LR, PST, COMPRESS, PLO> {
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
         Storage<LR, PST, COMPRESS, PLO> storage,
         const WEIGHTS& weights
         ):
      CoordUniformState<LR, PST, COMPRESS, PLO>(std::move(storage)),
      m_weights(weights)
   { reset(); }

   void reset()
   {
	  CoordUniformState<LR, PST, COMPRESS, PLO>::reset();
	  m_state.clear();
	  // empty set
	  m_state[LatticeTester::Coordinates()] =
		 boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 1.0);
   }

   /**
    * Updates the current state using the specified row of the permuted matrix      
    * of kernel values.           
    * This corresponds to appending a component \f$a_j\f$ to the generating      
    * vector \f$\boldsymbol a = (a_1, \dots, a_{j-1})\f$.      
    * To each possible value of \f$a_j\f$ corresponds a distinct row of the      
    * matrix \f$\boldsymbol\Omega\f$ of kernel values.           
    * This increases the internal dimension counter.
    *
    * Computes
    * \f[
    *    \boldsymbol p_{\mathfrak u \cup \{s\}} =
    *       \boldsymbol\omega_s \odot \boldsymbol p_{\mathfrak u}
    *       \quad (\forall \mathfrak u \subseteq \{1,\dots,s-1\}).
    * \f]
    */
   void update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen)
   {
      CoordUniformState<LR, PST, COMPRESS, PLO>::update(kernelValues, gen);

      using LatticeTester::Coordinates;

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
    * Computes and returns the weighted state vector \f$\boldsymbol q_s\f$.
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

	  using LatticeTester::Coordinates;

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

   /**
    * Returns a copy of this instance.
    */
   std::unique_ptr<CoordUniformState<LR, PST, COMPRESS, PLO>> clone() const
   { return std::unique_ptr<CoordUniformState<LR, PST, COMPRESS, PLO>>(new ConcreteCoordUniformState(*this)); }

private:
   const WEIGHTS& m_weights;

   // m_state[projection](i)
   std::map<LatticeTester::Coordinates, RealVector> m_state;
};


}}

#include "latbuilder/MeritSeq/ConcreteCoordUniformState-PD.h"
#include "latbuilder/MeritSeq/ConcreteCoordUniformState-OD.h"
#include "latbuilder/MeritSeq/ConcreteCoordUniformState-P.h"
#include "latbuilder/MeritSeq/ConcreteCoordUniformState-POD.h"

#endif
