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

#include "latbuilder/MeritSeq/ConcreteCoordUniformState-PD.h"

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// ProjectionDependentWeights
//========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
void
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, LatticeTester::ProjectionDependentWeights>::
reset()
{
   CoordUniformState<LR, ET, COMPRESS, PLO>::reset();
   m_state.clear();
   // empty set
   m_state[LatticeTester::Coordinates()] =
      boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 1.0);
   m_gen.clear();
}

//===========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
const RealVector&
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, LatticeTester::ProjectionDependentWeights>::
createStateVector(const LatticeTester::Coordinates& projection, const RealVector& kernelValues)
{
   auto it = m_state.find(projection);
   if (it != m_state.end())
      return it->second;

   // select largest index
   const LatticeTester::Coordinates::value_type largestCoord = *projection.rbegin();

   // remove largest coordinate index
   LatticeTester::Coordinates baseProjection = projection;
   baseProjection.erase(largestCoord);
   // create base state vector
   const RealVector& baseState = createStateVector(baseProjection, kernelValues);

   // compute merit value for new projection
   auto stridedKernelValues = this->storage().strided(
         kernelValues,
         m_gen[largestCoord]
         );
   return m_state[projection] = boost::numeric::ublas::element_prod(
         stridedKernelValues,
         baseState
         );
}

//===========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
void
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, LatticeTester::ProjectionDependentWeights>::
update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen)
{
   CoordUniformState<LR, ET, COMPRESS, PLO>::update(kernelValues, gen);
   m_gen.push_back(gen);

   // Create a new state vector for each projection $\mathfrak u$ such that
   // $\max \mathfrak u$ < $j$ and that $\mathfrak u \cup \{j\}$ has a nonzero
   // weight.
   const auto nextCoordinate = this->dimension();
   for (const auto& pw : m_weights.getWeightsForLargestIndex(nextCoordinate)) {
      // remove largest coordinate index
      LatticeTester::Coordinates proj = pw.first;
      proj.erase(*proj.rbegin());
      // create state vector
      createStateVector(proj, kernelValues);
   }
}

//===========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
RealVector
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, LatticeTester::ProjectionDependentWeights>::
weightedState() const
{
   using LatticeTester::Coordinates;

   const auto nextCoordinate = this->dimension();

   RealVector weightedState =
      boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 0.0);

   for (const auto& pw : m_weights.getWeightsForLargestIndex(nextCoordinate)) {
      // remove largest coordinate index
      LatticeTester::Coordinates proj = pw.first;
      proj.erase(*proj.rbegin());
      // find state vector
      auto it = m_state.find(proj);
      if (it == m_state.end())
         throw std::runtime_error("projection-dependent state was not created");
      // contribute to weighted state
      weightedState += pw.second * it->second;
   }

   return weightedState;
}

//===========================================================================

template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProjectionDependentWeights>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC, LatticeTester::ProjectionDependentWeights>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,      LatticeTester::ProjectionDependentWeights>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC, LatticeTester::ProjectionDependentWeights>;

template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProjectionDependentWeights>;
template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,      LatticeTester::ProjectionDependentWeights>;


template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProjectionDependentWeights>;

template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProjectionDependentWeights>;
template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC, LatticeTester::ProjectionDependentWeights>;

}}
