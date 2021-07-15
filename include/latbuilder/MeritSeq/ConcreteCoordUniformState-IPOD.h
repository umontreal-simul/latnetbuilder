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

#ifndef LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_IPOD_H
#define LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_IPOD_H

#include "latbuilder/MeritSeq/CoordUniformState.h"
#include "latbuilder/Storage.h"

#include "latbuilder/Interlaced/IPODWeights.h"

#include <vector>

namespace LatBuilder { namespace MeritSeq {

// forward declaration
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS , PerLevelOrder PLO, class WEIGHTS> class ConcreteCoordUniformState;

/**
 * Implementation of CoordUniformState for IPOD weights.
 * For this type of weights, the information used to compute the weighted state \f$\boldsymbol q_s\f$ can be stored in:
 * \f[
 *  \boldsymbol p_{j,l} = \sum_{\substack{\mathfrak{u} \subset \{1, \dots, jd\} \\ |w(u)|=l }} \delta_\mathfrak{u} \gamma_\mathfrak{u} \boldsymbol p_\mathfrak{u}.
   \f]
 * Recall that \f$d\f$ is the interlacing factor and \f$ s \f$ the dimension of the point set.
 */
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
class ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>> :
   public CoordUniformState<LR, ET, COMPRESS, PLO> {
public:
   /**
    * Constructor.
    * 
    * Sets \f$\mathcal D_0 = 0\f$ and \f$\boldsymbol p_{0,\ell} =
    * \boldsymbol 1\f$.
    *
    * \param storage       Storage configuration.
    * \param weights       IPOD weights.
    */
     ConcreteCoordUniformState(
         const Storage<LR, ET, COMPRESS, PLO>& storage,
         const LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>& weights
         );

   void reset();

   /**
    * Updates the current state using the specified row of the permuted matrix      
    * of kernel values.           
    * For lattices, this corresponds to appending a component \f$a_j\f$ to the generating      
    * vector \f$\boldsymbol a = (a_1, \dots, a_{j-1})\f$.      
    * To each possible value of \f$a_j\f$ corresponds a distinct row of the      
    * matrix \f$\boldsymbol\Omega\f$ of kernel values.           
    * This increases the internal dimension counter.
    *
    * Computes
    * \f[
    *  \boldsymbol p_{j, l} = \boldsymbol p_{j-1, l} + \gamma_j \sum_{i=1}^{d} \left[ e_i^d(\delta_{(j-1)d+1} \boldsymbol \omega^{((j-1)d+1)}, \dots, \delta_{jd} \boldsymbol \omega^{(jd)}) \right] \odot \boldsymbol p_{j-1,l-1}.
    * \f]
    * where \f$e_i^{n}\f$ denotes the elementary symmetric polynomial of degree \f$i\f$ with \f$n\f$ variables. 
    */
   void update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen);

   /**
    * Computes and returns the weighted state vector \f$\boldsymbol q_s\f$.
    *
    * Computes
    * \f[
    * \boldsymbol q_{jd-k} = \gamma_j \delta_{jd-k} \sum_{i=0}^{d-1-k} \left[ e_i^{d-1-k}(\delta_{(j-1)d+1} \boldsymbol \omega^{((j-1)d+1)}, \dots, \delta_{jd-k-1} \boldsymbol \omega^{(jd-k-1)})\right]  \odot \sum_{l=0}^{j-1} \Gamma_{l+1} \boldsymbol p_{j-1, l}. 
    * \f]
    * where \f$e_i^{n}\f$ denotes the elementary symmetric polynomial of degree \f$i\f$ with \f$n\f$ variables. 
    */
   RealVector weightedState() const;

    /**
    * Returns a copy of this instance.
    */
   std::unique_ptr<CoordUniformState<LR, ET, COMPRESS, PLO>> clone() const
   { return std::unique_ptr<CoordUniformState<LR, ET, COMPRESS, PLO>>(new ConcreteCoordUniformState(*this)); }

private:
   const LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>& m_weights;
   unsigned int m_interlacingFactor;

   RealVector m_elemPolySum;
   RealVector m_partialWeightedState;
   RealVector m_waitingKernelValues;
   std::vector<RealVector> m_state;
};



template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
class ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>> :
   public CoordUniformState<LR, ET, COMPRESS, PLO> {
public:
     ConcreteCoordUniformState(
         const Storage<LR, ET, COMPRESS, PLO>& storage,
         const LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>& weights
         );

   void reset();
   void update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen);
   RealVector weightedState() const;
   std::unique_ptr<CoordUniformState<LR, ET, COMPRESS, PLO>> clone() const
   { return std::unique_ptr<CoordUniformState<LR, ET, COMPRESS, PLO>>(new ConcreteCoordUniformState(*this)); }

private:
   const LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>& m_weights;
   unsigned int m_interlacingFactor;

   RealVector m_elemPolySum; // equals the left sum in the formula for the weighted state q
   RealVector m_partialWeightedState; // equals the right sum in the formula for the weighted state q
   std::vector<RealVector> m_state;
};



template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
class ConcreteCoordUniformState<LR, ET, COMPRESS, PLO, LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>> :
   public CoordUniformState<LR, ET, COMPRESS, PLO> {
public:
     ConcreteCoordUniformState(
         const Storage<LR, ET, COMPRESS, PLO>& storage,
         const LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>& weights
         );

   void reset();
   void update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen);
   RealVector weightedState() const;
   std::unique_ptr<CoordUniformState<LR, ET, COMPRESS, PLO>> clone() const
   { return std::unique_ptr<CoordUniformState<LR, ET, COMPRESS, PLO>>(new ConcreteCoordUniformState(*this)); }

private:
   const LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>& m_weights;
   unsigned int m_interlacingFactor;

   RealVector m_elemPolySum; // equals the left sum in the formula for the weighted state q
   RealVector m_partialWeightedState; // equals the right sum in the formula for the weighted state q
   std::vector<RealVector> m_state;
};


extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC,  LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC,  LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>>;


extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>>;


extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>>;

extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,  LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IAAlpha>>;


extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC,  LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC,  LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>>;


extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>>;
extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>>;


extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>>;

extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>>;
extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,  LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>>;


extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC,  LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC,  LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>>;


extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>>;


extern template class ConcreteCoordUniformState<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>>;

extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC,       LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>>;
extern template class ConcreteCoordUniformState<LatticeType::DIGITAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC,  LatBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::ICAlpha>>;


}}

#endif
