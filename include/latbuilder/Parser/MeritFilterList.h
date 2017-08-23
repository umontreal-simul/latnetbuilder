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

#ifndef LATBUILDER__PARSER__MERIT_FILTER_LIST_H
#define LATBUILDER__PARSER__MERIT_FILTER_LIST_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/MeritFilterList.h"
#include "latbuilder/SizeParam.h"

#include "latcommon/Weights.h"

namespace LatBuilder { namespace Parser {

/**
 * Parser for merit transformation filters.
 *
 * Parses a vector of strings consisting of either filters or, for embedded
 * lattices, combiners.
 */
   template <Lattice LR>
struct MeritFilterList {

   static LatBuilder::MeritFilterList<LR, LatBuilder::LatType::ORDINARY> parse(
         const std::vector<std::string>& filters,
         const LatBuilder::SizeParam<LR, LatBuilder::LatType::ORDINARY>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType
         )
   {
      LatBuilder::MeritFilterList<LR, LatType::ORDINARY> f;
      parse(f, filters, sizeParam, weights, normType);
      return f;
   }

   static void parse(
         LatBuilder::MeritFilterList<LR, LatBuilder::LatType::ORDINARY>& list,
         const std::vector<std::string>& filters,
         const LatBuilder::SizeParam<LR, LatBuilder::LatType::ORDINARY>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType
         );

   static LatBuilder::MeritFilterList<LR, LatBuilder::LatType::EMBEDDED> parse(
         const std::vector<std::string>& filters,
         const std::vector<std::string>& multilevelFilters,
         const std::string& combiner,
         const LatBuilder::SizeParam<LR, LatBuilder::LatType::EMBEDDED>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType
         )
   {
      LatBuilder::MeritFilterList<LR, LatType::EMBEDDED> f;
      parse(f, filters, multilevelFilters, combiner, sizeParam, weights, normType);
      return f;
   }

   static void parse(
         LatBuilder::MeritFilterList<LR, LatBuilder::LatType::EMBEDDED>& list,
         const std::vector<std::string>& filters,
         const std::vector<std::string>& multilevelFilters,
         const std::string& combiner,
         const LatBuilder::SizeParam<LR, LatBuilder::LatType::EMBEDDED>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType
         );
};

extern template struct MeritFilterList<Lattice::INTEGRATION> ;
extern template struct MeritFilterList<Lattice::POLYNOMIAL> ;

}}

#endif
