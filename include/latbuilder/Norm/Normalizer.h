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

#ifndef LATBUILDER__NORM__NORMALIZER_H
#define LATBUILDER__NORM__NORMALIZER_H

#include "latbuilder/MeritFilterList.h"

#include <limits>
#include <memory>

namespace LatBuilder { namespace Norm {

/**
 * Generic normalizer for merit values.
 *
 * \tparam ET   Type of lattice.
 * \tparam NORM  Type of norm.
 */
template <LatticeType LR, EmbeddingType ET, class NORM>
class Normalizer;

/**
 * Specialization of EmbeddingType::Normalizer for ordinary lattices.
 */
template <LatticeType LR, class NORM>
class Normalizer<LR, EmbeddingType::UNILEVEL, NORM> : public BasicMeritFilter<LR, EmbeddingType::UNILEVEL> {
public:
   typedef Real MeritValue;
   typedef LatBuilder::LatDef<LR, EmbeddingType::UNILEVEL> LatDef;
   typedef NORM Norm;

   /**
    * Constructor.
    * 
    * \param norm          Normalization function.
    */
   Normalizer(Norm norm):
      m_norm(std::move(norm)),
      m_cachedNorm(0),
      m_cachedSizeParam(typename LatticeTraits<LR>::Modulus(0)),
      m_cachedDimension(0)
   {}

   /**
    * Normalizes the values in \c merit.
    *
    * Returns \c true.
    */
   MeritValue operator()(const MeritValue& merit, const LatDef& lat) const;

   const Norm& norm() const
   { return m_norm; }

   std::string name() const
   { return "normalizer: " + norm().name(); }

private:
   /// Normalization function.
   Norm m_norm;

   /// Cache parameters.
   mutable Real m_cachedNorm;
   mutable SizeParam<LR, EmbeddingType::UNILEVEL> m_cachedSizeParam;
   mutable Dimension m_cachedDimension;

   void updateCache(
         const SizeParam<LR, EmbeddingType::UNILEVEL>& sizeParam,
         const Dimension dimension
         ) const;
};

/**
 * Specialization of EmbeddingType::Normalizer for embedded lattices.
 */
template <LatticeType LR, class NORM>
class Normalizer<LR, EmbeddingType::MULTILEVEL, NORM> : public BasicMeritFilter<LR, EmbeddingType::MULTILEVEL> {
public:
   typedef RealVector MeritValue;
   typedef LatBuilder::LatDef<LR, EmbeddingType::MULTILEVEL> LatDef;
   typedef NORM Norm;

   /**
    
    * 
    * \param norm          Normalization function.
    */
   Normalizer(Norm norm):
      m_norm(std::move(norm)),
      m_cachedNorm(0),
      m_cachedSizeParam(typename LatticeTraits<LR>::Modulus(0)),
      m_cachedDimension(0)
   {}

   /**
    * Normalizes the values in \c merit.
    *
    * Returns \c true.
    */
   MeritValue operator()(const MeritValue& merit, const LatDef& lat) const;

   const Norm& norm() const
   { return m_norm; }

   std::string name() const
   { 
      int begin = -1;
      int end = -1;
      for (unsigned int i=0; i<m_levelWeights.size(); i++){
            if (m_levelWeights[i] != 0 && begin == -1){
                  begin = i;
            }
            if (m_levelWeights[i] == 0 && begin != -1 && end == -1){
                  end = i-1;
            }
      }
      if (end == -1){
            end = (int) m_levelWeights.size()-1;
      }
      return "normalizer: " + norm().name() + " - levels: " + std::to_string(begin) + "," + std::to_string(end); 
   }

   /**
    * Sets the per-level weights.
    *
    * \param levelWeights  Per-level weights.
    *
    * \remark
    * The per-level weights \f$c_m\f$ for \f$m=1,\dots,M\f$ are expected to
    * satisfy \f$\sum_{m=0}^M c_m \leq 1\f$.
    */
   void setWeights(RealVector levelWeights);

   const RealVector& weights() const
   { return m_levelWeights; }

private:
   /// Normalization function.
   Norm m_norm;

   /// Per-level weights.
   RealVector m_levelWeights;

   /// Cache parameters.
   mutable RealVector m_cachedNorm;
   mutable SizeParam<LR, EmbeddingType::MULTILEVEL> m_cachedSizeParam;
   mutable Dimension m_cachedDimension;

   /**
    * Checks that the per-level weights \f$c_m\f$ for \f$m=1,\dots,M\f$ \c
    * maxPower satisfy \f$\sum_{m=0}^M c_m \leq 1\f$.
    */
   static void checkWeights(const RealVector& levelWeights);

   void updateCache(
         const SizeParam<LR, EmbeddingType::MULTILEVEL>& sizeParam,
         const Dimension dimension,
         bool force = false
         ) const;
};

//================================================================================

template <LatticeType LR, class NORM>
Real Normalizer<LR, EmbeddingType::UNILEVEL, NORM>::operator()(
      const MeritValue& merit,
      const LatDef& lat) const
{
   updateCache(lat.sizeParam(), lat.dimension());
   return merit * m_cachedNorm;
}

//================================================================================

template <LatticeType LR, class NORM>
RealVector Normalizer<LR, EmbeddingType::MULTILEVEL, NORM>::operator()(
      const MeritValue& merit,
      const LatDef& lat) const
{
   updateCache(lat.sizeParam(), lat.dimension());

   if (merit.size() > m_levelWeights.size())
      throw std::invalid_argument("Normalizer::operator(): "
            "merit has more levels than the per-level weights");

   RealVector out(merit.size());
   for (RealVector::size_type j = 0; j < out.size(); j++)
      out[j] = m_cachedNorm[j] * merit[j];
   return out;
}

//================================================================================

template <LatticeType LR, class NORM>
void Normalizer<LR, EmbeddingType::UNILEVEL, NORM>::updateCache(
      const SizeParam<LR, EmbeddingType::UNILEVEL>& sizeParam,
      const Dimension dimension) const
{
   if (m_cachedSizeParam == sizeParam and m_cachedDimension == dimension)
      return;

   m_cachedSizeParam = sizeParam;
   m_cachedDimension = dimension;

   // pre-compute normalization factor

   // find the tightest bound
   // (step 4 in Algorithm 2 from \cite vDIC08c)
   m_cachedNorm = 1.0 / norm()(sizeParam, dimension, 1.0);
}

//================================================================================

template <LatticeType LR, class NORM>
void Normalizer<LR, EmbeddingType::MULTILEVEL, NORM>::updateCache(
      const SizeParam<LR, EmbeddingType::MULTILEVEL>& sizeParam,
      const Dimension dimension, bool force) const
{
   if (m_cachedSizeParam == sizeParam and m_cachedDimension == dimension)
      return;

   if (force and sizeParam.numPoints() == 0)
      return;

   m_cachedNorm.resize(m_levelWeights.size());

   // pre-compute normalization factors

   for (Level level = 0; level < m_cachedNorm.size(); level++) {

      SizeParam<LR, EmbeddingType::MULTILEVEL> levelSizeParam(
            sizeParam.base(),
            level
            );

      if (m_levelWeights[level] == 0.0)
         // if weights is zero, leave norm to zero
         m_cachedNorm[level] = 0.0;
      else
         // find the tightest bound
         // (step 4 in Algorithm 2 from \cite vDIC08c)
         m_cachedNorm[level] = 1.0 / norm()(
               levelSizeParam,
               dimension,
               m_levelWeights[level]
               );
   }

   m_cachedSizeParam = sizeParam;
   m_cachedDimension = dimension;
}

//================================================================================

template <LatticeType LR, class NORM>
void Normalizer<LR, EmbeddingType::MULTILEVEL, NORM>::setWeights(RealVector levelWeights)
{
   m_levelWeights = std::move(levelWeights);
   //checkWeights(m_levelWeights);
   updateCache(m_cachedSizeParam, m_cachedDimension, true);
}

//================================================================================

template <LatticeType LR, class NORM>
void Normalizer<LR, EmbeddingType::MULTILEVEL, NORM>::checkWeights(
      const RealVector& levelWeights)
{
   Real wsum = 0.0;
   for (const auto& w : levelWeights)
      wsum += w;
   if (wsum > 1.0 + levelWeights.size() * std::numeric_limits<Real>::epsilon())
      throw std::invalid_argument("the sum of the per-level weights is larger than 1");
}

}}

#endif
