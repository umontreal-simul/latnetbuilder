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

#ifndef LATBUILDER__PARSER__MERIT_COMBINER_H
#define LATBUILDER__PARSER__MERIT_COMBINER_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/MeritFilterList.h"
#include "latbuilder/SizeParam.h"

#include <memory>

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid combiner.
 */
class BadCombiner : public ParserError {
public:
   BadCombiner(const std::string& message):
      ParserError("cannot parse combiner string: " + message)
   {}
};


/**
 * Parser for filters combiners for use with embedded lattices.
 */
struct MeritCombiner {
   static std::unique_ptr<typename LatBuilder::MeritFilterList<LatType::EMBEDDED>::Combiner> parse(
         const std::string& str,
         const LatBuilder::SizeParam<LatType::EMBEDDED>& sizeParam
         );
};

}}

#endif

