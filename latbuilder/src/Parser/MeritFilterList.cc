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

#include "latbuilder/Parser/MeritFilterList.h"
#include "latbuilder/Parser/MeritCombiner.h"
#include "latbuilder/Parser/MeritFilter.h"
#include "latbuilder/MeritFilterList.h"
#include "latbuilder/SizeParam.h"

#include "latcommon/Weights.h"

namespace LatBuilder { namespace Parser {

void MeritFilterList::parse(
      LatBuilder::MeritFilterList<LatType::ORDINARY>& list,
      const std::vector<std::string>& filters,
      const LatBuilder::SizeParam<LatType::ORDINARY>& sizeParam,
      const LatCommon::Weights& weights
      )
{
   for (const auto& arg : filters)
      list.add(MeritFilter::parse(arg, sizeParam, weights));
}

void MeritFilterList::parse(
      LatBuilder::MeritFilterList<LatType::EMBEDDED>& list,
      const std::vector<std::string>& filters,
      const std::vector<std::string>& multilevelFilters,
      const std::string& combiner,
      const LatBuilder::SizeParam<LatType::EMBEDDED>& sizeParam,
      const LatCommon::Weights& weights
      )
{
   for (const auto& arg : filters)
      list.add(MeritFilter::parse(arg, sizeParam, weights));

   for (const auto& arg : multilevelFilters)
      list.add(MeritFilter::parse(arg, sizeParam, weights));

   list.add(MeritCombiner::parse(combiner, sizeParam));
}

}}
