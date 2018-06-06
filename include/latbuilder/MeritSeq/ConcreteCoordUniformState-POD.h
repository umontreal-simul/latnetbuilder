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

#ifndef LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_POD_H
#define LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_POD_H

#include "latbuilder/MeritSeq/CoordUniformState.h"
#include "latbuilder/Storage.h"

#include "latticetester/PODWeights.h"

#include <vector>

namespace LatBuilder { namespace MeritSeq {

// forward declaration
template <LatticeType LR, PointSetType PST, Compress COMPRESS , PerLevelOrder PLO, class WEIGHTS> class ConcreteCoordUniformState;

/**
 * Implementation of CoordUniformState for POD weights.
 */
template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO>
class ConcreteCoordUniformState<LR, PST, COMPRESS, PLO, LatticeTester::PODWeights> :
   public CoordUniformState<LR, PST, COMPRESS, PLO> {
public:
   /**
    * Constructor.
    * 
    * Sets \f$\mathcal D_0^2 = 0\f$ and \f$\boldsymbol p_{0,\ell} =
    * \boldsymbol 1\f$.
    *
    * \param storage       Storage configuration.
    * \param weights       POD weights
    */
   ConcreteCoordUniformState(
         const Storage<LR, PST, COMPRESS, PLO>& storage,
         const LatticeTester::PODWeights& weights
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
    *    \boldsymbol p_{s,\ell} =
    *       \boldsymbol p_{s-1,\ell} + \boldsymbol\omega_s \odot \boldsymbol p_{s-1,\ell-1}.
    * \f]
    */
   void update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen);

   /**
    * Computes and returns the weighted state vector \f$\boldsymbol q_s\f$.
    *
    * Computes
    * \f[
    *    \boldsymbol q_s = \sum_{\ell=0}^s \Gamma_{\ell+1} \boldsymbol p_{s,\ell}.
    * \f]
    */
   RealVector weightedState() const;

      /**
    * Returns a copy of this instance.
    */
   std::unique_ptr<CoordUniformState<LR, PST, COMPRESS, PLO>> clone() const
   { return std::unique_ptr<CoordUniformState<LR, PST, COMPRESS, PLO>>(new ConcreteCoordUniformState(*this)); }

private:
   const LatticeTester::PODWeights& m_weights;

   // m_state[level](i)
   std::vector<RealVector> m_state;
};


extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatticeTester::PODWeights>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC,  LatticeTester::PODWeights>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       LatticeTester::PODWeights>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC,  LatticeTester::PODWeights>;

extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, PointSetType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatticeTester::PODWeights>;
extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       LatticeTester::PODWeights>;


extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatticeTester::PODWeights>;

extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, PointSetType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatticeTester::PODWeights>;
extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,  LatticeTester::PODWeights>;

}}

#endif
