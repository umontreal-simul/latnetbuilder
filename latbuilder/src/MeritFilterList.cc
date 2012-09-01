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

#include "latbuilder/MeritFilterList.h"

namespace LatBuilder {

namespace {
   void setValue(Real& merit, Real value)
   { merit = value; }
   void setValue(RealVector& merit, Real value)
   { merit = boost::numeric::ublas::scalar_vector<Real>(merit.size(), value); }
}

template <LatType LAT>
auto BasicMeritFilterList<LAT>::apply(
      MeritValue merit,
      const LatDef& lat
      ) const -> MeritValue
{
   try {
      for (const auto& filter : filters())
         merit = (*filter)(merit, lat);
   }
   catch (LatticeRejectedException&) {
      onReject()(lat);
      setValue(merit, std::numeric_limits<Real>::infinity());
   }
   return merit;
}

template class BasicMeritFilterList<LatType::ORDINARY>;
template class BasicMeritFilterList<LatType::EMBEDDED>;


Real MeritFilterListPolicy<LatType::ORDINARY>::applyFilters(Real merit, const LatDef& lat) const
{ return OBase::apply(merit, lat); }

Real MeritFilterListPolicy<LatType::EMBEDDED>::applyFilters(const RealVector& merit, const typename EBase::LatDef& lat) const
{ return OBase::apply(combiner()(EBase::apply(merit, lat), lat), lat); }

namespace {
   template <typename LIST>
   void filtersFormatHelper(std::ostream& os, const LIST& list, const std::string& label)
   {
      os << label << " = (";
      auto it = list.begin();
      if (it != list.end())
         os << (*it)->name();
      while (++it != list.end())
         os << ", " << (*it)->name();
      os << ")";
   }
}

std::ostream& operator<<(std::ostream& os, const BasicMeritFilterList<LatType::ORDINARY>& filters)
{ filtersFormatHelper(os, filters.filters(), "scalar filters"); return os; }

std::ostream& operator<<(std::ostream& os, const BasicMeritFilterList<LatType::EMBEDDED>& filters)
{ filtersFormatHelper(os, filters.filters(), "multilevel filters"); return os; }

std::ostream& operator<<(std::ostream& os, const MeritFilterList<LatType::ORDINARY>& filters)
{ return os << dynamic_cast<const BasicMeritFilterList<LatType::ORDINARY>&>(filters); return os; }

std::ostream& operator<<(std::ostream& os, const MeritFilterList<LatType::EMBEDDED>& filters)
{
   os << dynamic_cast<const BasicMeritFilterList<LatType::ORDINARY>&>(filters);
   os << ", ";
   os << "combiner=" << filters.combiner().name();
   os << ", ";
   os << dynamic_cast<const BasicMeritFilterList<LatType::EMBEDDED>&>(filters);
   return os;
}

}
