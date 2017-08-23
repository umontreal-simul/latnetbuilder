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


template <Lattice LR, LatType LAT>
struct MeritFilter {
   
   static std::unique_ptr<BasicMeritFilter<LR, LAT>> parse(
         const std::string& str,
         const LatBuilder::SizeParam<LR, LAT>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType);
};

extern template struct LatBuilder::Parser::MeritFilter <Lattice::INTEGRATION, LatType::ORDINARY> ;
extern template struct LatBuilder::Parser::MeritFilter <Lattice::INTEGRATION, LatType::EMBEDDED> ;
extern template struct LatBuilder::Parser::MeritFilter <Lattice::POLYNOMIAL, LatType::ORDINARY> ;
extern template struct LatBuilder::Parser::MeritFilter <Lattice::POLYNOMIAL, LatType::EMBEDDED> ;



}}

#endif
