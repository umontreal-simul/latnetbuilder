// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#include "latbuilder/MeritSeq/ConcreteCoordUniformState-POD.h"

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// PODWeights
//========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
void
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, LatticeTester::PODWeights>::
reset()
{
   CoordUniformState<LR, ET, COMPRESS, PLO>::reset();
   m_state.clear();
   // order 0
   m_state.push_back(RealVector(this->storage().size(), 1.0));
}

//===========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
void
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, LatticeTester::PODWeights>::
update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen)
{
   CoordUniformState<LR, ET, COMPRESS, PLO>::update(kernelValues, gen);

   auto stridedKernelValues = this->storage().strided(kernelValues, gen);

   const auto newCoordinate = this->dimension() - 1;

   const Real pweight = m_weights.getProductWeights().getWeightForCoordinate(newCoordinate);

   // add new order
   m_state.push_back(RealVector(this->storage().size(), 0.0));

   // recursive update by decreasing order to avoid unwanted overwriting
   for (size_t order = m_state.size() - 1; order > 0; order--)
      m_state[order] += boost::numeric::ublas::element_prod(
            pweight * stridedKernelValues,
            m_state[order - 1]
            );
}

//===========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
RealVector
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, LatticeTester::PODWeights>::
weightedState() const
{
   using LatticeTester::Coordinates;

   const auto nextCoordinate = this->dimension();

   const Real pweight = m_weights.getProductWeights().getWeightForCoordinate(nextCoordinate);

   RealVector weightedState =
      boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 0.0);

   for (Coordinates::size_type order = 0; order < m_state.size(); order++) {

      Real weight = m_weights.getOrderDependentWeights().getWeightForOrder(order + 1);

      if (weight == 0.0)
         continue;

      weightedState += weight * m_state[order];
   }

   return pweight * weightedState;
}


template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatticeTester::PODWeights>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC,  LatticeTester::PODWeights>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       LatticeTester::PODWeights>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC,  LatticeTester::PODWeights>;

template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatticeTester::PODWeights>;
template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       LatticeTester::PODWeights>;


template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatticeTester::PODWeights>;

template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatticeTester::PODWeights>;
template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,  LatticeTester::PODWeights>;

}}
