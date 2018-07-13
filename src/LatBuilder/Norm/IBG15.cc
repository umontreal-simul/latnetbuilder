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

#include "latbuilder/Norm/IBG15.h"
#include "latbuilder/WeightsDispatcher.h"
#include "latbuilder/Util.h"

#include <vector>
#include <cmath>
#include <boost/math/tools/polynomial.hpp>

namespace LatBuilder { namespace Norm {

namespace {

 typedef boost::math::tools::polynomial<double> RealPolynomial;

   template <typename WEIGHTS>
   struct SumHelper {
      Real operator()(
            const WEIGHTS& weights,
            Real lambda,
            Dimension dimension,
            unsigned int alpha
            ) const
      {
         throw std::runtime_error("IBG15 normalization not implemented for these weights.");
         return 1.;
      }
   };


#define DECLARE_IBG15_SUM(weight_type) \
      template <> \
      class SumHelper<weight_type> { \
      public: \
         Real operator()( \
               const weight_type& weights, \
               Real lambda, \
               Dimension dimension, \
               unsigned int alpha \
               ) const; \
      }\

   DECLARE_IBG15_SUM(LatBuilder::CombinedWeights);
   DECLARE_IBG15_SUM(NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>);
   DECLARE_IBG15_SUM(NetBuilder::Interlaced::IPDWeights<LatBuilder::Kernel::IB>);

#undef DECLARE_IBG15_SUM

   //===========================================================================
   // combined weights
   //===========================================================================

   // Separating sumCombined() from
   // SumHelper<LatBuilder::CombinedWeights>::operator() is a workaround for
   // LLVM/clang++.
   Real sumCombined(
         const CombinedWeights& weights,
         Real lambda,
         Dimension dimension,
         unsigned int alpha
         )
   {
      Real val = 0.0;
      for (const auto& w : weights.list())
         val += WeightsDispatcher::dispatch<SumHelper>(*w, lambda, dimension, alpha);
      return val;
   }

   Real SumHelper<LatBuilder::CombinedWeights>::operator()(
         const CombinedWeights& weights,
         Real lambda,
         Dimension dimension,
         unsigned int alpha
         ) const
   {
      return sumCombined(weights, lambda, dimension, alpha);
   }


   //===========================================================================
   // interlaced projection-dependent weights
   //===========================================================================

   Real SumHelper<NetBuilder::Interlaced::IPDWeights<LatBuilder::Kernel::IB>>::operator()(
         const NetBuilder::Interlaced::IPDWeights<LatBuilder::Kernel::IB>& weights,
         Real lambda,
         Dimension dimension,
         unsigned int alpha
         ) const
   { 
      unsigned int interlacingFactor = weights.interlacingFactor();
      Dimension j0 = (Dimension) std::ceil( (double) dimension / interlacingFactor);
      Dimension d0 = dimension - (j0 - 1) * interlacingFactor;
      Real gTilde = std::max(pow(intPow(2.0,std::min(alpha, interlacingFactor))-2,-lambda), 1 / (pow(2.0, lambda * std::min(alpha, interlacingFactor)) - 2));

      Real g0 = 1;
      for(Dimension l = 1; l <= d0; ++l)
      {
            g0 *= 1 + pow(2.0, lambda * (interlacingFactor - l)) * gTilde;
      }
      Real g = g0;
      g0 -= 1;
      for(Dimension l = d0 + 1; l <= interlacingFactor; ++l)
      {
            g *= 1 + pow(2.0, lambda * (interlacingFactor - l)) * gTilde;
      }
      g -= 1.0;
      Real val = 0.0;
      for (Dimension largestIndex = 0; largestIndex < j0; largestIndex++) {
         // iterate only through projections that have a weight
         for (const auto& pw : weights.getBaseWeights().getWeightsForLargestIndex(largestIndex)) {
            const auto& proj = pw.first;
            const auto& weight = pw.second;
            if (weight)
            {
                  if (largestIndex < j0 - 1)
                  {
                        val += pow(weight, lambda) * intPow(g, proj.size());
                  }
                  else
                  {
                        val += pow(weight, lambda) * g0 * intPow(g, proj.size() - 1);
                  }
            }
         }
      }

      return val;
   }

   //===========================================================================
   // IPOD weights
   //===========================================================================

   Real SumHelper<NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>>::operator()(
         const NetBuilder::Interlaced::IPODWeights<LatBuilder::Kernel::IB>& weights,
         Real lambda,
         Dimension dimension,
         unsigned int alpha
         ) const
   {
      unsigned int interlacingFactor = weights.interlacingFactor();
      Dimension j0 = (Dimension) std::ceil( (double) dimension / interlacingFactor);
      Dimension d0 = dimension - (j0 - 1) * interlacingFactor;
      Real gTilde = std::max(pow(intPow(2.0,std::min(alpha, interlacingFactor))-2,-lambda), 1 / (pow(2.0, lambda * std::min(alpha, interlacingFactor)) - 2));

      Real g0 = 1;
      for(Dimension l = 1; l <= d0; ++l)
      {
            g0 *= 1 + pow(2.0, lambda * (interlacingFactor - l)) * gTilde;
      }
      Real g = g0;
      g0 -= 1;
      for(Dimension l = d0 + 1; l <= interlacingFactor; ++l)
      {
            g *= 1 + pow(2.0, lambda * (interlacingFactor - l)) * gTilde;
      }
      g -= 1.0;

      Real val = g0 * pow(weights.getWeightForCoordinate(j0 - 1) * weights.getWeightForOrder(1), lambda);
      RealPolynomial acc{1.0};
      for(Dimension coord = 0; coord < j0 - 1; ++coord)
      {
            acc *= RealPolynomial{{pow(weights.getWeightForCoordinate(coord), lambda) * g, 1.0}};
      }
      for(Dimension degree = 0; degree < j0 - 1; ++degree)
      {
            val += acc[degree] * (pow(weights.getWeightForOrder(j0 - 1 - degree),lambda) + g0 * pow(weights.getWeightForCoordinate(j0 - 1) * weights.getWeightForOrder(j0 - degree), lambda)) ;
      }
      return val;
  }

}

IBG15::IBG15(unsigned int alpha, const LatticeTester::Weights& weights, Real normType):
   NormAlphaBase<IBG15>(alpha, normType),
   m_weights(weights)
{}

template <LatticeType LR, EmbeddingType L>
Real IBG15::value(
      Real lambda,
      const SizeParam<LR, L>& sizeParam,
      Dimension dimension,
      Real norm
      ) const
{
   norm = 1.0 / (sizeParam.numPoints() - 1.0);
   Real val = WeightsDispatcher::dispatch<SumHelper>(
         m_weights,
         lambda,
         dimension,
         alpha()
         );

   return pow(val * norm, 1.0 / lambda);
}

template Real IBG15::value<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>(Real, const SizeParam<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>&, Dimension, Real) const;
template Real IBG15::value<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>(Real, const SizeParam<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>&, Dimension, Real) const;

template Real IBG15::value<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>(Real, const SizeParam<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>&, Dimension, Real) const;
template Real IBG15::value<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>(Real, const SizeParam<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>&, Dimension, Real) const;

}}
