// Copyright (c) 2012, 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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
      struct SumHelper<weight_type> { \
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

   Real SumHelper<LatBuilder::CombinedWeights>::operator()(
         const CombinedWeights& weights,
         Real normType,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      Real val = 0.0;
      for (const auto& w : weights.list())
         val += WeightsDispatcher::dispatch<SumHelper>(*w, normType, z, lambda, dimension);
      return val;
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

template <LatType L>
Real PAlphaDPW08::value(
      Real lambda,
      const SizeParam<L>& sizeParam,
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

template Real PAlphaDPW08::value<LatType::ORDINARY>(Real, const SizeParam<LatType::ORDINARY>&, Dimension, Real) const;
template Real PAlphaDPW08::value<LatType::EMBEDDED>(Real, const SizeParam<LatType::EMBEDDED>&, Dimension, Real) const;

}}
