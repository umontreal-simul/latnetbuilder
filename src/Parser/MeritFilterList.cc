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
      const LatCommon::Weights& weights,
      Real normType
      )
{
   for (const auto& arg : filters)
      list.add(MeritFilter::parse(arg, sizeParam, weights, normType));
}

void MeritFilterList::parse(
      LatBuilder::MeritFilterList<LatType::EMBEDDED>& list,
      const std::vector<std::string>& filters,
      const std::vector<std::string>& multilevelFilters,
      const std::string& combiner,
      const LatBuilder::SizeParam<LatType::EMBEDDED>& sizeParam,
      const LatCommon::Weights& weights,
      Real normType
      )
{
   for (const auto& arg : filters)
      list.add(MeritFilter::parse(arg, sizeParam, weights, normType));

   for (const auto& arg : multilevelFilters)
      list.add(MeritFilter::parse(arg, sizeParam, weights, normType));

   list.add(MeritCombiner::parse(combiner, sizeParam));
}

}}
