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

#ifndef E_MERIT_FILTER_H
#define E_MERIT_FILTER_H

#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"
#include "latbuilder/MeritFilterList.h"

namespace LatBuilder
{

/**
 * Generic filter wrapper for merit values.
 *
 * \tparam MERIT  Type of merit value.
 *
 * For embedded lattices, the filter is applied to each per-level merit value
 * independently.  For ordinary lattices, the filer is applied to the single
 * merit value.
 *
 * This class template is useful to reuse the same code to apply the filter to a
 * single merit value or to embedded merit values.
 */
template <LatticeType LR, LatEmbed LAT>
class MeritFilter : public BasicMeritFilter<LR, LAT> {
public:
   typedef typename BasicMeritFilter<LR, LAT>::InputMeritValue  MeritValue;
   typedef typename BasicMeritFilter<LR, LAT>::LatDef           LatDef;

   /**
    * Filter function type.
    * Expected to return \c true if and only if the merit value is accepted.
    */
   typedef std::function<bool (Real&)> Filter;

   /**
    * Constructor.
    *
    * \param filter Filter callable.
    * \param name   Description string for the filter.
    */
   MeritFilter(Filter filter, std::string name = "<unknown filter>"):
      m_filter(std::move(filter)),
      m_name(std::move(name))
   {}
   
   /**
    * Filters the merit value \c merit.
    */
   MeritValue operator() (const MeritValue& merit, const LatDef& lat) const
   {
      MeritValue out = merit;
      if (not applyFilter<MeritValue>(out, nullptr))
         throw LatticeRejectedException();
      return out;
   }

   std::string name() const
   { return m_name; }

private:
   Filter m_filter;
   std::string m_name;

   /**
    * Applies \c f to each element of \c x.
    */
   template <typename T>
   bool applyFilter(T& x, typename T::iterator*) const
   {
      bool ret = true;
      for (auto& xi : x)
         ret &= m_filter(xi);
      return ret;
   }

   /**
    * Applies \c f to \c x.
    */
   template <typename T>
   bool applyFilter(T& x, ...) const
   { return m_filter(x); }
};

}

#endif
