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

#include "latbuilder/Norm/PAlphaDPW08.h"
#include "latbuilder/WeightsDispatcher.h"
#include "latbuilder/Util.h"

#include <boost/math/special_functions/zeta.hpp>

namespace LatBuilder { namespace Norm {

namespace {

   template <typename WEIGHTS>
   class SumHelper {
   public:
      Real operator()(
            const WEIGHTS& weights,
            Real normType,
            Real z,
            Real lambda,
            Dimension dimension
            ) const
      { throw std::logic_error("DPW08 norm not implemented for this type of weights"); }
   };

#define DECLARE_PALPHA_DPW08_SUM(weight_type) \
      template <> \
      class SumHelper<weight_type> { \
      public: \
         Real operator()( \
               const weight_type& weights, \
               Real normType, \
               Real z, \
               Real lambda, \
               Dimension dimension \
               ) const; \
      }

   DECLARE_PALPHA_DPW08_SUM(LatCommon::ProductWeights);
   DECLARE_PALPHA_DPW08_SUM(LatBuilder::CombinedWeights);

#undef DECLARE_PALPHA_DPW08_SUM

   //===========================================================================
   // combined weights
   //===========================================================================

   // Separating sumCombined() from
   // SumHelper<LatBuilder::CombinedWeights>::operator() is a workaround for
   // LLVM/clang++.
   Real sumCombined(
         const CombinedWeights& weights,
         Real normType,
         Real z,
         Real lambda,
         Dimension dimension
         )
   {
      Real val = 0.0;
      for (const auto& w : weights.list())
         val += WeightsDispatcher::dispatch<SumHelper>(*w, normType, z, lambda, dimension);
      return val;
   }

   Real SumHelper<LatBuilder::CombinedWeights>::operator()(
         const CombinedWeights& weights,
         Real normType,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      return sumCombined(weights, normType, z, lambda, dimension);
   }


   //===========================================================================
   // product weights
   //===========================================================================

   Real SumHelper<LatCommon::ProductWeights>::operator()(
         const LatCommon::ProductWeights& weights,
         Real normType,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      Real val = 1.0;
      for (Dimension coord = 0; coord < dimension; coord++) {
         Real weight = weights.getWeightForCoordinate(coord);
         if (weight)
            // weights are assumed to already be to the power normType; map
            // them to power 2
            val *= 1.0 + z * pow(weight, lambda * 2 / normType);
      }
      val -= 1.0;
      return val;
   }
}

PAlphaDPW08::PAlphaDPW08(unsigned int alpha, const LatCommon::Weights& weights, Real normType):
   PAlphaBase<PAlphaDPW08>(alpha, normType),
   m_weights(weights)
{}

template <LatticeType LR, PointSetType L>
Real PAlphaDPW08::value(
      Real lambda,
      const SizeParam<LR, L>& sizeParam,
      Dimension dimension,
      Real norm
      ) const
{
   norm = 1.0 / (norm * sizeParam.numPoints());
   const auto kappa = primeFactors(sizeParam.numPoints()).size();
   // 2^(kappa+1), where kappa is the number of distinct prime factors of n
   const auto k = intPow(2, kappa + 1);
   Real z = static_cast<Real>(k * boost::math::zeta<Real>(this->alpha() * lambda));
   Real val = WeightsDispatcher::dispatch<SumHelper>(
         m_weights,
         this->normType(),
         z,
         lambda,
         dimension
         );

   return pow(norm * val, 1.0 / lambda);
}

template Real PAlphaDPW08::value<LatticeType::ORDINARY, PointSetType::UNILEVEL>(Real, const SizeParam<LatticeType::ORDINARY, PointSetType::UNILEVEL>&, Dimension, Real) const;
template Real PAlphaDPW08::value<LatticeType::ORDINARY, PointSetType::MULTILEVEL>(Real, const SizeParam<LatticeType::ORDINARY, PointSetType::MULTILEVEL>&, Dimension, Real) const;

template Real PAlphaDPW08::value<LatticeType::POLYNOMIAL, PointSetType::UNILEVEL>(Real, const SizeParam<LatticeType::POLYNOMIAL, PointSetType::UNILEVEL>&, Dimension, Real) const;
template Real PAlphaDPW08::value<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL>(Real, const SizeParam<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL>&, Dimension, Real) const;

}}
