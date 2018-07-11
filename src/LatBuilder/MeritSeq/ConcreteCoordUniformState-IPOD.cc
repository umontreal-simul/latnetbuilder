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

#include "latbuilder/MeritSeq/ConcreteCoordUniformState-IPOD.h"

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// IPODWeights
//========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class KERNEL>
void
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<KERNEL>>::
reset()
{
   CoordUniformState<LR, ET, COMPRESS, PLO>::reset();
   m_state.clear();
   m_ODSum.clear();
   m_ElemPolySum.clear();
   // order 0
   m_state.push_back(RealVector(this->storage().size(), 1.0));
   m_ODSum = RealVector(this->storage().size(), 0.);
   m_ElemPolySum = RealVector(this->storage().size(), 0.);
}

//===========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class KERNEL>
void
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<KERNEL>>::
update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen)
{
   CoordUniformState<LR, ET, COMPRESS, PLO>::update(kernelValues, gen);
   const auto newCoordinate = this->dimension() - 1;
   auto stridedKernelValues = this->storage().strided(kernelValues, gen);
   Real dweight = m_weights.getCorrectionProductWeightForCoordinate(newCoordinate);
   m_ElemPolySum = boost::numeric::ublas::element_prod(
      m_ElemPolySum,
      RealVector(this->storage().size(), 1.) + dweight * stridedKernelValues
      );

   if (this->m_dimension % m_interlacingFactor == 0){
      m_ElemPolySum -= RealVector(this->storage().size(), 1.);

      const Real pweight = m_weights.getWeightForCoordinate(newCoordinate % m_interlacingFactor);

      // add new order
      m_state.push_back(RealVector(this->storage().size(), 0.0));

      // recursive update by decreasing order to avoid unwanted overwriting
      for (size_t order = m_state.size() - 1; order > 0; order--)
            m_state[order] += boost::numeric::ublas::element_prod(
                  pweight * m_ElemPolySum,
                  m_state[order - 1]
                  );

      m_ElemPolySum = RealVector(this->storage().size(), 0.);
      m_ODSum = RealVector(this->storage().size(), 0.);
      for (size_t order = 0; order < m_state.size(); order--){
            m_ODSum += m_weights.getWeightForOrder(order+1) * m_state[order];
      }

   }

}

//===========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class KERNEL>
RealVector
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<KERNEL>>::
weightedState() const
{
   using LatticeTester::Coordinates;

   const auto nextCoordinate = this->dimension();

   const Real pweight = m_weights.getWeightForCoordinate(nextCoordinate % m_interlacingFactor);
   const Real dweight = m_weights.getCorrectionProductWeightForCoordinate(nextCoordinate);

   return boost::numeric::ublas::element_prod(
         pweight * dweight * m_ElemPolySum,
         m_ODSum
   );
}


extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<Kernel::AIDNAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       NetBuilder::Interlaced::IPODWeights<Kernel::AIDNAlpha>>;

extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<Kernel::AIDNAlpha>>;

extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<Kernel::AIDNAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,  NetBuilder::Interlaced::IPODWeights<Kernel::AIDNAlpha>>;


extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<Kernel::BIDN>>;
extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       NetBuilder::Interlaced::IPODWeights<Kernel::BIDN>>;

extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<Kernel::BIDN>>;

extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<Kernel::BIDN>>;
extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,  NetBuilder::Interlaced::IPODWeights<Kernel::BIDN>>;

}}
