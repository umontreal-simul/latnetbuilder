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

#include "latbuilder/MeritFilterList.h"

namespace LatBuilder {

namespace {
   void setValue(Real& merit, Real value)
   { merit = value; }
   void setValue(RealVector& merit, Real value)
   { merit = boost::numeric::ublas::scalar_vector<Real>(merit.size(), value); }
}

template <LatticeType LR, EmbeddingType ET>
auto BasicMeritFilterList<LR, ET>::apply(
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

template class BasicMeritFilterList<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>;
template class BasicMeritFilterList<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>;
template class BasicMeritFilterList<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>;
template class BasicMeritFilterList<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>;

template< LatticeType LR>
Real MeritFilterListPolicy<LR, EmbeddingType::UNILEVEL>::applyFilters(Real merit, const LatDef<LR,EmbeddingType::UNILEVEL>& lat) const
{ return OBase::apply(merit, lat); }

template< LatticeType LR>
Real MeritFilterListPolicy<LR, EmbeddingType::MULTILEVEL>::applyFilters(const RealVector& merit, const typename EBase::LatDef& lat) const
{ return OBase::apply(combiner()(EBase::apply(merit, lat), lat), lat); }

template class MeritFilterListPolicy<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>;
template class MeritFilterListPolicy<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>;
template class MeritFilterListPolicy<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>;
template class MeritFilterListPolicy<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>;

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

template<>
std::ostream& operator<<(std::ostream& os, const BasicMeritFilterList<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>& filters)
{ filtersFormatHelper(os, filters.filters(), "scalar filters"); return os; }

template<>
std::ostream& operator<<(std::ostream& os, const BasicMeritFilterList<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>& filters)
{ filtersFormatHelper(os, filters.filters(), "multilevel filters"); return os; }

template<>
std::ostream& operator<<(std::ostream& os, const MeritFilterList<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>& filters)
{ return os << dynamic_cast<const BasicMeritFilterList<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>&>(filters); return os; }

template<>
std::ostream& operator<<(std::ostream& os, const MeritFilterList<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>& filters)
{
   os << dynamic_cast<const BasicMeritFilterList<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>&>(filters);
   os << ", ";
   os << "combiner=" << filters.combiner().name();
   os << ", ";
   os << dynamic_cast<const BasicMeritFilterList<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>&>(filters);
   return os;
}

//===============================================================================================================

template<>
std::ostream& operator<<(std::ostream& os, const BasicMeritFilterList<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>& filters)
{ filtersFormatHelper(os, filters.filters(), "scalar filters"); return os; }

template<>
std::ostream& operator<<(std::ostream& os, const BasicMeritFilterList<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>& filters)
{ filtersFormatHelper(os, filters.filters(), "multilevel filters"); return os; }

template<>
std::ostream& operator<<(std::ostream& os, const MeritFilterList<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>& filters)
{ return os << dynamic_cast<const BasicMeritFilterList<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>&>(filters); return os; }

template<>
std::ostream& operator<<(std::ostream& os, const MeritFilterList<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>& filters)
{
   os << dynamic_cast<const BasicMeritFilterList<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>&>(filters);
   os << ", ";
   os << "combiner=" << filters.combiner().name();
   os << ", ";
   os << dynamic_cast<const BasicMeritFilterList<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>&>(filters);
   return os;
}




}
