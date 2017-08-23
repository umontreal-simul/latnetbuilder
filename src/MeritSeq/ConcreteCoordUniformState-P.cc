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

#include "latbuilder/MeritSeq/ConcreteCoordUniformState-P.h"

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// ProductWeights
//========================================================================

template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
void
ConcreteCoordUniformState<LR, LAT, COMPRESS, PLO, LatCommon::ProductWeights>::
reset()
{
   CoordUniformState<LR, LAT, COMPRESS, PLO>::reset();
   m_state.clear();
   m_state = boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 1.0);
}

//===========================================================================

template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
void
ConcreteCoordUniformState<LR, LAT, COMPRESS, PLO, LatCommon::ProductWeights>::
update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen)
{
   CoordUniformState<LR, LAT, COMPRESS, PLO>::update(kernelValues, gen);

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

template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
RealVector
ConcreteCoordUniformState<LR, LAT, COMPRESS, PLO, LatCommon::ProductWeights>::
weightedState() const
{
   const auto nextCoordinate = this->dimension();

   const Real weight = m_weights.getWeightForCoordinate(nextCoordinate);

   return weight * m_state;
}

template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::ORDINARY, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::ProductWeights>;
template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::ORDINARY, Compress::SYMMETRIC, PerLvlOrder::BASIC, LatCommon::ProductWeights>;
template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::CYCLIC,      LatCommon::ProductWeights>;
template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::EMBEDDED, Compress::SYMMETRIC, PerLvlOrder::CYCLIC, LatCommon::ProductWeights>;

template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::ORDINARY, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::ProductWeights>;
template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::CYCLIC,      LatCommon::ProductWeights>;


template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::ProductWeights>;
}}
