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

#include "latbuilder/Norm/PAlphaPLR.h"
#include "latbuilder/WeightsDispatcher.h"
#include "latbuilder/Util.h"

#include <boost/math/special_functions/binomial.hpp>
#include <boost/math/tools/polynomial.hpp>
#include <vector>
#include <cmath>

namespace LatBuilder { namespace Norm {

namespace {

   template <typename WEIGHTS>
   struct SumHelper {
      Real operator()(
            const WEIGHTS& weights,
            Real normType,
            Real lambda,
            Dimension dimension,
            unsigned int alpha
            ) const
      {
         std::cerr << "warning: using default implementation of SumHelper" << std::endl;
         Real val = 0.0;
         Real mu = pow(2.0, alpha * lambda) / (pow(2.0, alpha * lambda) - 2);
         LatticeTester::CoordinateSets::FromRanges csets(1, dimension, 0, dimension - 1);
         for (const auto& proj : csets) {
            Real weight = weights.getWeight(proj);
            if (weight)
               // weights are assumed to already be to the power normType; map
               // them to power 2
               val += intPow(mu, proj.size()) * pow(weight, lambda * 2 / normType);
         }
         return val;
      }
   };


#define DECLARE_PALPHA_PLR_SUM(weight_type) \
      template <> \
      class SumHelper<weight_type> { \
      public: \
         Real operator()( \
               const weight_type& weights, \
               Real normType, \
               Real lambda, \
               Dimension dimension, \
               unsigned int alpha \
               ) const; \
      }\

   DECLARE_PALPHA_PLR_SUM(LatticeTester::ProjectionDependentWeights);
   DECLARE_PALPHA_PLR_SUM(LatticeTester::OrderDependentWeights);
   DECLARE_PALPHA_PLR_SUM(LatticeTester::ProductWeights);
   DECLARE_PALPHA_PLR_SUM(LatticeTester::PODWeights);
   DECLARE_PALPHA_PLR_SUM(LatBuilder::CombinedWeights);

#undef DECLARE_PALPHA_PLR_SUM

   //===========================================================================
   // combined weights
   //===========================================================================

   // Separating sumCombined() from
   // SumHelper<LatBuilder::CombinedWeights>::operator() is a workaround for
   // LLVM/clang++.
   Real sumCombined(
         const CombinedWeights& weights,
         Real normType,
         Real lambda,
         Dimension dimension,
         unsigned int alpha
         )
   {
      Real val = 0.0;
      for (const auto& w : weights.list())
         val += WeightsDispatcher::dispatch<SumHelper>(*w, normType, lambda * 2 / normType, dimension, alpha);
      return val;
   }

   Real SumHelper<LatBuilder::CombinedWeights>::operator()(
         const CombinedWeights& weights,
         Real normType,
         Real lambda,
         Dimension dimension,
         unsigned int alpha
         ) const
   {
      return sumCombined(weights, normType, lambda, dimension, alpha);
   }


   //===========================================================================
   // projection-dependent weights
   //===========================================================================

   Real SumHelper<LatticeTester::ProjectionDependentWeights>::operator()(
         const LatticeTester::ProjectionDependentWeights& weights,
         Real normType,
         Real lambda,
         Dimension dimension,
         unsigned int alpha
         ) const
   {
      Real val = 0.0;
      Real mu = pow(2.0, alpha * lambda) / (pow(2.0, alpha * lambda) - 2);
      for (Dimension largestIndex = 0; largestIndex < dimension; largestIndex++) {
         // iterate only through projections that have a weight
         for (const auto& pw : weights.getWeightsForLargestIndex(largestIndex)) {
            const auto& proj = pw.first;
            const auto& weight = pw.second;
            if (weight)
               // weights are assumed to already be to the power normType; map
               // them to power 2
               val += intPow(mu, proj.size()) * pow(weight, lambda * 2 / normType);
         }
      }
      return val;
   }


   //===========================================================================
   // order-dependent weights
   //===========================================================================

   Real SumHelper<LatticeTester::OrderDependentWeights>::operator()(
         const LatticeTester::OrderDependentWeights& weights,
         Real normType,
         Real lambda,
         Dimension dimension,
         unsigned int alpha
         ) const
   {
      Real val = 0.0;
      Real mu = pow(2.0, alpha * lambda) / (pow(2.0, alpha * lambda) - 2);
      for (Dimension order = 1; order <= dimension; order++) {
         Real weight = weights.getWeightForOrder(order);
         if (weight)
               // weights are assumed to already be to the power normType; map
               // them to power 2
            val += boost::math::binomial_coefficient<double>((unsigned int)dimension, (unsigned int)order) * std::pow(weight, lambda * 2.0 / normType) * intPow(mu, order);
      }
      return val;
   }


   //===========================================================================
   // product weights
   //===========================================================================

   Real SumHelper<LatticeTester::ProductWeights>::operator()(
         const LatticeTester::ProductWeights& weights,
         Real normType,
         Real lambda,
         Dimension dimension,
         unsigned int alpha
         ) const
   {
      Real val = 1.0;
      Real mu = pow(2.0, alpha * lambda) / (pow(2.0, alpha * lambda) - 2);
      for (Dimension coord = 0; coord < dimension; coord++) {
         Real weight = weights.getWeightForCoordinate(coord);
         if (weight)
               // weights are assumed to already be to the power normType; map
               // them to power 2
            val *= 1.0 + mu * std::pow(weight, lambda * 2 / normType);
      }
      val -= 1.0;
      return val;
   }


   //===========================================================================
   // POD weights
   //===========================================================================

   Real SumHelper<LatticeTester::PODWeights>::operator()(
         const LatticeTester::PODWeights& weights,
         Real normType,
         Real lambda,
         Dimension dimension,
         unsigned int alpha
         ) const
   {
      Real mu = pow(2.0, alpha * lambda) / (pow(2.0, alpha * lambda) - 2);

      typedef boost::math::tools::polynomial<double> RealPolynomial;

      RealPolynomial acc{1.0};
      Real val = 0.0;
      for(Dimension coord = 0; coord < dimension; ++coord)
      {
            acc *= RealPolynomial{{pow(weights.getWeightForCoordinate(coord), lambda * 2 / normType) * mu, 1.0}};
      }
      for(Dimension degree = 0; degree < dimension; ++degree)
      {
            val += acc[degree] * pow(weights.getWeightForOrder(dimension - degree), lambda * 2 / normType) ;
      }
      return val;
   }

}

PAlphaPLR::PAlphaPLR(unsigned int alpha, const LatticeTester::Weights& weights, Real normType):
   NormAlphaBase<PAlphaPLR>(alpha, normType),
   m_weights(weights)
{}

template <LatticeType LR, EmbeddingType L>
Real PAlphaPLR::value(
      Real lambda,
      const SizeParam<LR, L>& sizeParam,
      Dimension dimension,
      Real norm
      ) const
{
   norm = 2.0 / (norm * sizeParam.numPoints());
   Real val = WeightsDispatcher::dispatch<SumHelper>(
         m_weights,
         this->normType(),
         lambda,
         dimension,
         alpha()
         );

   return std::pow(norm * val, 1.0 / lambda);
}

template Real PAlphaPLR::value<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>(Real, const SizeParam<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>&, Dimension, Real) const;
template Real PAlphaPLR::value<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>(Real, const SizeParam<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>&, Dimension, Real) const;

template Real PAlphaPLR::value<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>(Real, const SizeParam<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>&, Dimension, Real) const;
template Real PAlphaPLR::value<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>(Real, const SizeParam<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>&, Dimension, Real) const;

}}
