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
