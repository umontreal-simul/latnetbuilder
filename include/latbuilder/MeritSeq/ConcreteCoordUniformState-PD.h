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

#include "latticetester/ProjectionDependentWeights.h"
#include "latticetester/Coordinates.h"

#include <map>


namespace LatBuilder { namespace MeritSeq {

// forward declaration
template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class WEIGHTS > class ConcreteCoordUniformState;

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
template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO>
class ConcreteCoordUniformState<LR, PST, COMPRESS, PLO, LatticeTester::ProjectionDependentWeights> :
   public CoordUniformState<LR, PST, COMPRESS, PLO> {
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
         const Storage<LR, PST, COMPRESS, PLO>& storage,
         const LatticeTester::ProjectionDependentWeights& weights
         ):
      CoordUniformState<LR, PST, COMPRESS, PLO>(storage),
      m_weights(weights)
   { reset(); }

   void reset();

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
   void update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen);

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
   RealVector weightedState() const;

    /**
    * Returns a copy of this instance.
    */
   std::unique_ptr<CoordUniformState<LR, PST, COMPRESS, PLO>> clone() const
   { return std::unique_ptr<CoordUniformState<LR, PST, COMPRESS, PLO>>(new ConcreteCoordUniformState(*this)); }

private:
   const LatticeTester::ProjectionDependentWeights& m_weights;

   // m_state[projection](i)
   // declared mutable because it is updated transparently by #getStateVector()
   std::map<LatticeTester::Coordinates, RealVector> m_state;

   // keep track of the selected generator values to be able to generate state
   // vectors on demand
   GeneratingVector m_gen;

   /**
    * Creates and stores the state vector for \c projection if it has not been
    * created yet.
    *
    * \return A reference to the state vector.
    */
   const RealVector& createStateVector(const LatticeTester::Coordinates& projection, const RealVector& kernelValues);
};



extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProjectionDependentWeights>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC, LatticeTester::ProjectionDependentWeights>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,      LatticeTester::ProjectionDependentWeights>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC, LatticeTester::ProjectionDependentWeights>;

extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, PointSetType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProjectionDependentWeights>;
extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,      LatticeTester::ProjectionDependentWeights>;


extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProjectionDependentWeights>;

extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, PointSetType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProjectionDependentWeights>;
extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC, LatticeTester::ProjectionDependentWeights>;

}}

#endif
