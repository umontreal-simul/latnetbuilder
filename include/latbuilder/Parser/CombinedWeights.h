// Copyright (c) 2012, 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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

#ifndef LATBUILDER__PARSER__COMBINED_WEIGHTS_H
#define LATBUILDER__PARSER__COMBINED_WEIGHTS_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Types.h"

#include "latbuilder/CombinedWeights.h"

#include <memory>
#include <vector>

namespace LatBuilder { namespace Parser {

/**
 * Parser for weights.
 */
struct CombinedWeights {
   /**
    * Parses a string specifying a file containing a description of combined weights.
    *
    * Adds projection-dependent and order-dependent weights to an existing
    * CombinedWeights instance.
    *
    * Example strings: <code>file:filename</code>
    *
    * \return \c true on success; \c false on failure
    */
   static bool
   parseFile(const std::string& arg, LatBuilder::CombinedWeights& weights, Real powerScale);

   /**
    * Parses a vector of strings specifying weights.
    *
    * For example strings, see #parseFile(),
    * Parser::Weights::parseProjectionDependent(),
    * Parser::Weights::parseOrderDependent() and
    * Parser::Weights::parseProduct().
    *
    * \return A pointer to a newly created object or \c nullptr on failure.
    */
   static std::unique_ptr<LatBuilder::CombinedWeights>
   parse(const std::vector<std::string>& args, Real powerScale);
};

}}

#endif
