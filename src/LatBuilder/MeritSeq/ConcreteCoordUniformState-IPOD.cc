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

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>::
ConcreteCoordUniformState(
         const Storage<LR, ET, COMPRESS, PLO>& storage,
         const NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>& weights
         ):
      CoordUniformState<LR, ET, COMPRESS, PLO>(storage),
      m_weights(weights),
      m_interlacingFactor(m_weights.getInterlacingFactor())
   { reset();}

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
void
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>::
reset()
{
   CoordUniformState<LR, ET, COMPRESS, PLO>::reset();
   m_state.clear();
   m_ODSum.clear();
   m_ElemPolySum.clear();
   m_state.push_back(RealVector(this->storage().size(), 1.0));
   m_ODSum = RealVector(this->storage().size(), m_weights.getWeightForOrder(1));
   m_ElemPolySum = RealVector(this->storage().size(), 1.);
   m_waitingKernelValues = RealVector(this->storage().size(), 1.);
}

//===========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
void
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>::
update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen)
{
      std::cout << "update with new coord=" << this->dimension() << std::endl;
      
   CoordUniformState<LR, ET, COMPRESS, PLO>::update(kernelValues, gen);
   const auto newCoordinate = this->dimension() - 1;

//    if (newCoordinate % m_interlacingFactor == d-1){
      
//    }

   auto stridedKernelValues = this->storage().strided(kernelValues, gen);
   Real dweight = m_weights.getCorrectionProductWeightForCoordinate(newCoordinate);
   m_ElemPolySum = boost::numeric::ublas::element_prod(
      m_ElemPolySum,
      RealVector(this->storage().size(), 1.) + dweight * stridedKernelValues
      );
//    m_waitingKernelValues = ;

   if (newCoordinate % m_interlacingFactor == m_interlacingFactor-1){
      // RealVector nextElemPolySum = boost::numeric::ublas::element_prod(
      // m_ElemPolySum,
      // m_waitingKernelValues
      // );
      m_ElemPolySum -= RealVector(this->storage().size(), 1.);

      const Real pweight = m_weights.getWeightForCoordinate(newCoordinate / m_interlacingFactor);

      
      m_state.push_back(RealVector(this->storage().size(), 0.0));

      for (size_t order = m_state.size() - 1; order > 0; order--)
            m_state[order] += boost::numeric::ublas::element_prod(
                  pweight * m_ElemPolySum,
                  m_state[order - 1]
                  );
      
      std::cout << "elem poly sum" << std::endl;
      for (unsigned int i=0; i<m_ElemPolySum.size(); i++){
            std::cout << m_ElemPolySum[i] << " ";
      }

      m_ODSum = RealVector(this->storage().size(), 0.);
      std::cout << "m_state" << std::endl;
      std::cout << m_state.size() << std::endl;
      for (size_t order = 0; order < m_state.size(); order++){
            for (unsigned int i=0; i<m_state[order].size(); i++){
                  std::cout << m_state[order][i] << " ";
            }
            std::cout << std::endl;
            m_ODSum += m_weights.getWeightForOrder(order+1) * m_state[order];
      }
      m_ElemPolySum = RealVector(this->storage().size(), 1.);

   }
}

//===========================================================================

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
RealVector
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>::
weightedState() const
{
   std::cout << "begin weighted state" << std::endl;
   using LatticeTester::Coordinates;

   const auto nextCoordinate = this->dimension();

   const Real pweight = m_weights.getWeightForCoordinate(nextCoordinate / m_interlacingFactor);
   const Real dweight = m_weights.getCorrectionProductWeightForCoordinate(nextCoordinate);

   auto res = boost::numeric::ublas::element_prod(
         pweight * dweight * m_ElemPolySum,
         m_ODSum
   );
   std::cout << "elem poly sum" << std::endl;
   for (unsigned int i=0; i<m_ElemPolySum.size(); i++){
         std::cout << m_ElemPolySum[i] << " ";
   }
   std::cout << std::endl;
   std::cout << "od sum" << std::endl;
   for (unsigned int i=0; i<m_ODSum.size(); i++){
         std::cout << m_ODSum[i] << " ";
   }
   std::cout << std::endl;
   return boost::numeric::ublas::element_prod(
         pweight * dweight * m_ElemPolySum,
         m_ODSum
   );
}

template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC,  NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>;
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC,  NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>;

template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>;
template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>;


template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>;

template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>;
template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,  NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::AIDNAlpha>>;

}}
