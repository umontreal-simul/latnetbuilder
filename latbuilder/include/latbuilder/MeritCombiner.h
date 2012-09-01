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
 * \tparam ACCTAG          Tag from boost::accumulators that defines the
 *                         operator \f$\mathcal K\f$ on the set elements.
 *                         Typically tag::sum or tag::max.
 */
namespace MeritCombiner {

/**
 * Functor that sums the merit values of embedded lattices to produce a compound
 * merit value.
 *
 */
template <template <typename> class ACC>
class Accumulator : public MeritFilterList<LatType::EMBEDDED>::Combiner {
public:
   /**
    * Returns the accumulated merit value for \c merit.
    */
   Real operator() (const RealVector& merit, const LatDef& lat) const
   {
      LatBuilder::Accumulator<ACC, Real> acc(0.0);
      for (const auto x : merit)
         acc.accumulate(x);
      return acc.value();
   }

   std::string name() const
   { return LatBuilder::Accumulator<ACC, Real>::name(); }
};


/**
 * Functor that selects a the merit value of a specific embedded level as a
 * single merit value.
 */
class SelectLevel : public MeritFilterList<LatType::EMBEDDED>::Combiner {
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
   Real operator() (const RealVector& merit, const LatDef&) const
   { return merit[m_level]; }

   std::string name() const
   { return "select level " + boost::lexical_cast<std::string>(m_level); }

private:
   Level m_level;
};

}}

#endif
