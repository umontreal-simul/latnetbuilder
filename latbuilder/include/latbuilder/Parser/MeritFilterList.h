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
struct MeritFilterList {

   static LatBuilder::MeritFilterList<LatBuilder::LatType::ORDINARY> parse(
         const std::vector<std::string>& filters,
         const LatBuilder::SizeParam<LatBuilder::LatType::ORDINARY>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType
         )
   {
      LatBuilder::MeritFilterList<LatType::ORDINARY> f;
      parse(f, filters, sizeParam, weights, normType);
      return f;
   }

   static void parse(
         LatBuilder::MeritFilterList<LatBuilder::LatType::ORDINARY>& list,
         const std::vector<std::string>& filters,
         const LatBuilder::SizeParam<LatBuilder::LatType::ORDINARY>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType
         );

   static LatBuilder::MeritFilterList<LatBuilder::LatType::EMBEDDED> parse(
         const std::vector<std::string>& filters,
         const std::vector<std::string>& multilevelFilters,
         const std::string& combiner,
         const LatBuilder::SizeParam<LatBuilder::LatType::EMBEDDED>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType
         )
   {
      LatBuilder::MeritFilterList<LatType::EMBEDDED> f;
      parse(f, filters, multilevelFilters, combiner, sizeParam, weights, normType);
      return f;
   }

   static void parse(
         LatBuilder::MeritFilterList<LatBuilder::LatType::EMBEDDED>& list,
         const std::vector<std::string>& filters,
         const std::vector<std::string>& multilevelFilters,
         const std::string& combiner,
         const LatBuilder::SizeParam<LatBuilder::LatType::EMBEDDED>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType
         );
};

}}

#endif
