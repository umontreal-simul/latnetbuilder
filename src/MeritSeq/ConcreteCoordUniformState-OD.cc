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

#include "latbuilder/MeritSeq/ConcreteCoordUniformState-OD.h"

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// OrderDependentWeights
//========================================================================

template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
void
ConcreteCoordUniformState<LR, LAT, COMPRESS, PLO, LatCommon::OrderDependentWeights>::
reset()
{
   CoordUniformState<LR, LAT, COMPRESS, PLO>::reset();
   m_state.clear();
   // order 0
   m_state.push_back(RealVector(this->storage().size(), 1.0));
}

//===========================================================================

template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
void
ConcreteCoordUniformState<LR, LAT, COMPRESS, PLO, LatCommon::OrderDependentWeights>::
update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen)
{
   CoordUniformState<LR, LAT, COMPRESS, PLO>::update(kernelValues, gen);

   auto stridedKernelValues = this->storage().strided(kernelValues, gen);

   // add new order
   m_state.push_back(RealVector(this->storage().size(), 0.0));

   // recursive update by decreasing order to avoid unwanted overwriting
   for (size_t order = m_state.size() - 1; order > 0; order--)
      m_state[order] += boost::numeric::ublas::element_prod(
            stridedKernelValues,
            m_state[order - 1]
            );
}

//===========================================================================

template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
RealVector
ConcreteCoordUniformState<LR, LAT, COMPRESS, PLO, LatCommon::OrderDependentWeights>::
weightedState() const
{
   using LatCommon::Coordinates;

   RealVector weightedState =
      boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 0.0);

   for (Coordinates::size_type order = 0; order < m_state.size(); order++) {

      Real weight = m_weights.getWeightForOrder(order + 1);

      if (weight == 0.0)
         continue;

      weightedState += weight * m_state[order];
   }

   return weightedState;
}

template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::ORDINARY, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::OrderDependentWeights>;
template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::ORDINARY, Compress::SYMMETRIC, PerLvlOrder::BASIC, LatCommon::OrderDependentWeights>;
template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::CYCLIC,      LatCommon::OrderDependentWeights>;
template class ConcreteCoordUniformState<Lattice::INTEGRATION, LatType::EMBEDDED, Compress::SYMMETRIC, PerLvlOrder::CYCLIC, LatCommon::OrderDependentWeights>;

template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::ORDINARY, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::OrderDependentWeights>;
template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::CYCLIC,      LatCommon::OrderDependentWeights>;


template class ConcreteCoordUniformState<Lattice::POLYNOMIAL, LatType::EMBEDDED, Compress::NONE, PerLvlOrder::BASIC,      LatCommon::OrderDependentWeights>;

}}
