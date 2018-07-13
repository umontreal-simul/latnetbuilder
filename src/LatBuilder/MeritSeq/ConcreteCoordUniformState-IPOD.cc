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
#include "latbuilder/TextStream.h"
#include <iostream>

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// IPODWeights
//========================================================================

#define CONCRETE_COORD_UNIF_STATE(KERNEL)\
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>\
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<KERNEL>>::\
ConcreteCoordUniformState(\
         const Storage<LR, ET, COMPRESS, PLO>& storage,\
         const NetBuilder::Interlaced::IPODWeights<KERNEL>& weights\
         ):\
      CoordUniformState<LR, ET, COMPRESS, PLO>(storage),\
      m_weights(weights),\
      m_interlacingFactor(m_weights.interlacingFactor())\
   {\
      if (LR == LatticeType::ORDINARY)\
      {\
       throw std::runtime_error("Interlacing is not implemented for ordinary lattice rules.");\
      }\
      reset();\
   }\
\
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>\
void \
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<KERNEL>>::\
reset()\
{\
   CoordUniformState<LR, ET, COMPRESS, PLO>::reset();\
   m_state.clear();\
   m_partialWeightedState.clear(); \
   m_elemPolySum.clear();\
   m_state.push_back(RealVector(this->storage().size(), 1.0));\
   m_partialWeightedState = RealVector(this->storage().size(), m_weights.getWeightForOrder(1));\
   m_elemPolySum = RealVector(this->storage().size(), 1.); /*the first elementary symmetric polynomial equals 1*/\
}\
\
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>\
void \
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<KERNEL>>::\
update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen)\
{\
\
   CoordUniformState<LR, ET, COMPRESS, PLO>::update(kernelValues, gen);\
   const auto newCoordinate = this->dimension() - 1;\
\
   auto stridedKernelValues = this->storage().strided(kernelValues, gen);\
   Real dweight = m_weights.getCorrectionProductWeightForCoordinate(newCoordinate);\
   m_elemPolySum = boost::numeric::ublas::element_prod(\
      m_elemPolySum,\
      RealVector(this->storage().size(), 1.) + dweight * stridedKernelValues\
      );\
   if (newCoordinate % m_interlacingFactor == m_interlacingFactor-1){\
      /*we are changing of `real` coordinate*/\
      m_elemPolySum -= RealVector(this->storage().size(), 1.);\
\
      const Real pweight = m_weights.getWeightForCoordinate(newCoordinate / m_interlacingFactor);\
\
      \
      m_state.push_back(RealVector(this->storage().size(), 0.0));\
\
      for (size_t order = m_state.size() - 1; order > 0; order--)\
            m_state[order] += boost::numeric::ublas::element_prod(\
                  pweight * m_elemPolySum,\
                  m_state[order - 1]\
                  );\
\
      m_partialWeightedState = RealVector(this->storage().size(), 0.);\
      for (size_t order = 0; order < m_state.size(); order++){\
            m_partialWeightedState += m_weights.getWeightForOrder(order+1) * m_state[order];\
      }\
      m_elemPolySum = RealVector(this->storage().size(), 1.);\
\
   }\
}\
\
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>\
RealVector \
ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, NetBuilder::Interlaced::IPODWeights<KERNEL>>::\
weightedState() const\
{\
   using LatticeTester::Coordinates;\
\
   const auto nextCoordinate = this->dimension();\
\
   const Real pweight = m_weights.getWeightForCoordinate(nextCoordinate / m_interlacingFactor);\
   const Real dweight = m_weights.getCorrectionProductWeightForCoordinate(nextCoordinate);\
\
   return boost::numeric::ublas::element_prod(\
         pweight * dweight * m_elemPolySum,\
         m_partialWeightedState\
   );\
}\
\
/*Ordinary state must be instantiated for compatibility purposes but will throw a runtime error when constructed.*/\
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<KERNEL>>;\
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC,  NetBuilder::Interlaced::IPODWeights<KERNEL>>;\
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       NetBuilder::Interlaced::IPODWeights<KERNEL>>;\
template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC,  NetBuilder::Interlaced::IPODWeights<KERNEL>>;\
\
template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<KERNEL>>;\
template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       NetBuilder::Interlaced::IPODWeights<KERNEL>>;\
template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<KERNEL>>;\
\
template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       NetBuilder::Interlaced::IPODWeights<KERNEL>>;\
template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,  NetBuilder::Interlaced::IPODWeights<KERNEL>>;\


CONCRETE_COORD_UNIF_STATE(LatBuilder::Kernel::IAAlpha);
CONCRETE_COORD_UNIF_STATE(LatBuilder::Kernel::IB);

#undef CONCRETE_COORD_UNIF_STATE


}}
