// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
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

#include "latbuilder/MeritSeq/ConcreteCoordUniformState-P.h"

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// ProductWeights
//========================================================================

template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO>
void
ConcreteCoordUniformState<LR, PST, COMPRESS, PLO, LatticeTester::ProductWeights>::
reset()
{
   CoordUniformState<LR, PST, COMPRESS, PLO>::reset();
   m_state.clear();
   m_state = boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 1.0);
}

//===========================================================================

template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO>
void
ConcreteCoordUniformState<LR, PST, COMPRESS, PLO, LatticeTester::ProductWeights>::
update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen)
{
   CoordUniformState<LR, PST, COMPRESS, PLO>::update(kernelValues, gen);

   auto stridedKernelValues = this->storage().strided(kernelValues, gen);

   const auto newCoordinate = this->dimension() - 1;

   const Real weight = m_weights.getWeightForCoordinate(newCoordinate);

   const auto ones = boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 1.0);

   m_state = boost::numeric::ublas::element_prod(
         ones + weight * stridedKernelValues,
         m_state
         );
}

//===========================================================================

template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO>
RealVector
ConcreteCoordUniformState<LR, PST, COMPRESS, PLO, LatticeTester::ProductWeights>::
weightedState() const
{
   const auto nextCoordinate = this->dimension();

   const Real weight = m_weights.getWeightForCoordinate(nextCoordinate);

   return weight * m_state;
}

template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProductWeights>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC, LatticeTester::ProductWeights>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,      LatticeTester::ProductWeights>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC, LatticeTester::ProductWeights>;

template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, PointSetType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProductWeights>;
template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,      LatticeTester::ProductWeights>;


template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProductWeights>;

template class ConcreteCoordUniformState<LatticeType::DIGITAL, PointSetType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,      LatticeTester::ProductWeights>;
template class ConcreteCoordUniformState<LatticeType::DIGITAL, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC, LatticeTester::ProductWeights>;

}}
