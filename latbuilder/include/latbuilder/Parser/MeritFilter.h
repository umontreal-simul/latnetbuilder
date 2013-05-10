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

#ifndef LATBUILDER__PARSER__MERIT_FILTER_H
#define LATBUILDER__PARSER__MERIT_FILTER_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/BasicMeritFilter.h"

#include "latcommon/Weights.h"

#include <memory>

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid filter string.
 */
class BadFilter : public ParserError {
public:
   BadFilter(const std::string& message):
      ParserError("cannot parse filter string: " + message)
   {}
};

struct MeritFilter {
   template <LatType LAT>
   static std::unique_ptr<BasicMeritFilter<LAT>> parse(
         const std::string& str,
         const LatBuilder::SizeParam<LAT>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType);
};

extern template
std::unique_ptr<BasicMeritFilter<LatType::ORDINARY>>
MeritFilter::parse(
      const std::string&,
      const LatBuilder::SizeParam<LatType::ORDINARY>&,
      const LatCommon::Weights&,
      Real);

extern template
std::unique_ptr<BasicMeritFilter<LatType::EMBEDDED>>
MeritFilter::parse(
      const std::string&,
      const LatBuilder::SizeParam<LatType::EMBEDDED>&,
      const LatCommon::Weights&,
      Real);

}}

#endif
