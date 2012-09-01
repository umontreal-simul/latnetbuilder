// Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
// 
// This file is part of Lattice Builder.
// 
// Lattice Builder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Lattice Builder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

#include "latbuilder/Norm/PAlphaSL10.h"
#include "latbuilder/WeightsDispatcher.h"
#include "latbuilder/Util.h"

#include <boost/math/special_functions/zeta.hpp>
#include <vector>

namespace LatBuilder { namespace Norm {

namespace {

   template <typename WEIGHTS>
   struct SumHelper {
      Real operator()(
            const WEIGHTS& weights,
            Real z,
            Real lambda,
            Dimension dimension
            ) const
      {
         std::cerr << "warning: using default implementation of SumHelper" << std::endl;
         Real val = 0.0;
         LatCommon::CoordinateSets::FromRanges csets(1, dimension, 0, dimension - 1);
         for (const auto& proj : csets) {
            Real weight = weights.getWeight(proj);
            if (weight)
               val += intPow(z, proj.size()) * pow(weight, lambda);
         }
         return val;
      }
   };


#define DECLARE_PALPHA_SL10_SUM(weight_type) \
      template <> \
      struct SumHelper<weight_type> { \
      public: \
         Real operator()( \
               const weight_type& weights, \
               Real z, \
               Real lambda, \
               Dimension dimension \
               ) const; \
      }

   DECLARE_PALPHA_SL10_SUM(LatCommon::ProjectionDependentWeights);
   DECLARE_PALPHA_SL10_SUM(LatCommon::OrderDependentWeights);
   DECLARE_PALPHA_SL10_SUM(LatCommon::ProductWeights);
   DECLARE_PALPHA_SL10_SUM(LatCommon::PODWeights);
   DECLARE_PALPHA_SL10_SUM(CombinedWeights);

#undef DECLARE_PALPHA_SL10_SUM

   //===========================================================================
   // combined weights
   //===========================================================================

   Real SumHelper<CombinedWeights>::operator()(
         const CombinedWeights& weights,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      Real val = 0.0;
      for (const auto& w : weights.list())
         val += WeightsDispatcher::dispatch<SumHelper>(*w, z, lambda, dimension);
      return val;
   }


   //===========================================================================
   // projection-dependent weights
   //===========================================================================

   Real SumHelper<LatCommon::ProjectionDependentWeights>::operator()(
         const LatCommon::ProjectionDependentWeights& weights,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      Real val = 0.0;
      for (Dimension largestIndex = 0; largestIndex < dimension; largestIndex++) {
         // iterate only through projections that have a weight
         for (const auto& pw : weights.getWeightsForLargestIndex(largestIndex)) {
            const auto& proj = pw.first;
            const auto& weight = pw.second;
            if (weight)
               val += intPow(z, proj.size()) * pow(weight, lambda);
         }
      }
      return val;
   }


   //===========================================================================
   // order-dependent weights
   //===========================================================================

   Real SumHelper<LatCommon::OrderDependentWeights>::operator()(
         const LatCommon::OrderDependentWeights& weights,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      Real val = 0.0;
      Real cumul = 1.0;
      for (Dimension order = 1; order <= dimension; order++) {
         Real weight = weights.getWeightForOrder(order);
         cumul *= (dimension - order + 1) * z / order;
         if (weight)
            val += cumul * pow(weight, lambda);
      }
      return val;
   }


   //===========================================================================
   // product weights
   //===========================================================================

   Real SumHelper<LatCommon::ProductWeights>::operator()(
         const LatCommon::ProductWeights& weights,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      Real val = 1.0;
      for (Dimension coord = 0; coord < dimension; coord++) {
         Real weight = weights.getWeightForCoordinate(coord);
         if (weight)
            val *= 1.0 + z * pow(weight, lambda);
      }
      val -= 1.0;
      return val;
   }


   //===========================================================================
   // POD weights
   //===========================================================================

   Real SumHelper<LatCommon::PODWeights>::operator()(
         const LatCommon::PODWeights& weights,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      // compute states
      std::vector<Real> states;
      states.push_back(1.0);
      for (Dimension s = 1; s <= dimension; s++) {
         Real pweight = pow(weights.getProductWeights().getWeightForCoordinate(s), lambda);
         states.push_back(0.0);
         for (Dimension order = states.size() - 1; order > 0; order--)
            states[order] += z * pweight * states[order - 1];
      }
      Real val = 0.0;
      for (Dimension order = 1; order <= dimension; order++)
         val += pow(weights.getOrderDependentWeights().getWeightForOrder(order), lambda) * states[order];
      return val;
   }

}

PAlphaSL10::PAlphaSL10(unsigned int alpha, const LatCommon::Weights& weights):
   PAlphaBase<PAlphaSL10>(alpha),
   m_weights(weights)
{}

template <LatType L>
Real PAlphaSL10::value(
      Real lambda,
      const SizeParam<L>& sizeParam,
      Dimension dimension,
      Real norm
      ) const
{
   norm = 1.0 / (norm * sizeParam.totient());
   Real z = static_cast<Real>(2 * boost::math::zeta<Real>(this->alpha() * lambda));
   Real val = WeightsDispatcher::dispatch<SumHelper>(
         m_weights,
         z,
         lambda,
         dimension
         );

   return pow(norm * val, 1.0 / lambda);
}

template Real PAlphaSL10::value<LatType::ORDINARY>(Real, const SizeParam<LatType::ORDINARY>&, Dimension, Real) const;
template Real PAlphaSL10::value<LatType::EMBEDDED>(Real, const SizeParam<LatType::EMBEDDED>&, Dimension, Real) const;

}}
