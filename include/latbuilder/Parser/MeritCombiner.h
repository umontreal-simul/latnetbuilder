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
template <LatticeType LR>
struct MeritCombiner {
   static std::unique_ptr<typename LatBuilder::MeritFilterList<LR, LatEmbed::EMBEDDED>::Combiner> parse(
         const std::string& str,
         const LatBuilder::SizeParam<LR, LatEmbed::EMBEDDED>& sizeParam
         );
};

extern template struct MeritCombiner<LatticeType::ORDINARY>;
extern template struct MeritCombiner<LatticeType::POLYNOMIAL>;

}}

#endif

