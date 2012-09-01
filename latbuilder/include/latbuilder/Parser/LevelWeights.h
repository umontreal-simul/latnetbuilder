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

#ifndef LATBUILDER__PARSER__LEVEL_WEIGHTS_H
#define LATBUILDER__PARSER__LEVEL_WEIGHTS_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/SizeParam.h"

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid filter.
 */
class BadLevelWeights : public ParserError {
public:
   BadLevelWeights(const std::string& message):
      ParserError("cannot parse level weights: " + message)
   {}
};

/**
 * Parser for per-level weights.
 */
struct LevelWeights {
   typedef RealVector result_type;
   /**
    * Parses per-level weights.
    *
    * The weights are all even and the string must specify the minimum and
    * maximum levels.
    *
    * Example string: <tt>even</tt>, <tt>even:5</tt>, <tt>even:5,12</tt>
    *
    * Empty string is interpreted as <tt>even</tt>.
    *
    * \todo This is too limited a parser and needs to be improved.
    */
   static result_type parse(
         const std::string& arg,
         const SizeParam<LatType::EMBEDDED>& sizeParam
         );
};

}}

#endif
