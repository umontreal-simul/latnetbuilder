// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#ifndef E_MERIT_COMBINER_H
#define E_MERIT_COMBINER_H

#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"
#include "latbuilder/Accumulator.h"
#include "latbuilder/MeritFilterList.h"

#include <boost/lexical_cast.hpp>

#include <numeric>
#include <memory>

namespace LatBuilder
{

/**
 * Combiner functors for merit values.
 *
 * Designed to be used to combined the per-level merit values for embedded
 * lattices into a compound merit value.
 *
 * \tparam ACC             Tag from boost::accumulators that defines the
 *                         operator \f$\mathcal K\f$ on the set elements.
 *                         Typically tag::sum or tag::max.
 */
namespace MeritCombiner {

/**
 * Functor that sums the merit values of embedded lattices to produce a compound
 * merit value.
 *
 */
template <LatticeType LR, template <typename> class ACC>
class Accumulator : public MeritFilterList<LR, EmbeddingType::MULTILEVEL>::Combiner {
public:
   typedef typename MeritFilterList<LR, EmbeddingType::MULTILEVEL>::Combiner Combiner;
   typedef typename Combiner::LatDef LatDef;

   /**
    * Returns the accumulated merit value for \c merit.
    */
   Real operator() (const RealVector& merit, const LatDef& lat) const
   {
      LatBuilder::Accumulator<ACC, Real> acc(0.0);
      for (const auto x : merit){
         acc.accumulate(x);
       }
      return acc.value();
   }

   std::string name() const
   { return LatBuilder::Accumulator<ACC, Real>::name(); }
};


/**
 * Functor that selects a the merit value of a specific embedded level as a
 * single merit value.
 */
template <LatticeType LR>
class SelectLevel : public MeritFilterList<LR, EmbeddingType::MULTILEVEL>::Combiner {
public:
   /**
    * Constructor.
    */
   SelectLevel(Level level):
      m_level(level)
   {}
   /**
    * Calls the functor.
    */
   Real operator() (const RealVector& merit, const LatDef<LR, EmbeddingType::MULTILEVEL>&) const
   { return merit[m_level]; }

   std::string name() const
   { return "select level " + boost::lexical_cast<std::string>(m_level); }

private:
   Level m_level;
};

}}

#endif
