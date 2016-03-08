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
template <LatType LAT>
class MeritFilter : public BasicMeritFilter<LAT> {
public:
   typedef typename BasicMeritFilter<LAT>::InputMeritValue  MeritValue;
   typedef typename BasicMeritFilter<LAT>::LatDef           LatDef;

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
