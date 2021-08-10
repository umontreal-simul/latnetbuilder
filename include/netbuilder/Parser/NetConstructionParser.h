// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
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

#ifndef NETBUILDER__PARSER__NET_CONSTRUCTION_PARSER_H
#define NETBUILDER__PARSER__NET_CONSTRUCTION_PARSER_H

#include "latbuilder/Parser/Common.h"
#include "netbuilder/Types.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace NetBuilder { namespace Parser {
namespace lbp = LatBuilder::Parser;
/**
 * Exception thrown when trying to parse an invalid construction type.
 */
class BadNetConstruction : public lbp::ParserError {
public:
   BadNetConstruction(const std::string& message):
      lbp::ParserError("cannot parse construction string: " + message)
   {}
};

/**
 * Parser for construction types.
 */
template <EmbeddingType ET>
struct NetConstructionParser {};

template<>
struct NetConstructionParser<EmbeddingType::UNILEVEL>
{
   typedef NetBuilder::NetConstruction result_type;

   static result_type parse(const std::string& str)
   {
      if (str == "sobol")
      {
        return NetBuilder::NetConstruction::SOBOL;
      }
      else if (str == "polynomial")
      {
        return NetBuilder::NetConstruction::POLYNOMIAL;
      }
      else if (str == "explicit")
      {
        return NetBuilder::NetConstruction::EXPLICIT;
      }
      else if (str == "lms")
      {
        return NetBuilder::NetConstruction::LMS;
      }
      else
      {
        throw BadNetConstruction(str);
      }
   }
};

template<>
struct NetConstructionParser<EmbeddingType::MULTILEVEL>
{
   typedef NetBuilder::NetConstruction result_type;

   static result_type parse(const std::string& str)
   {

      result_type tmp = NetConstructionParser<EmbeddingType::UNILEVEL>::parse(str);

      if (tmp != NetConstruction::SOBOL && tmp != NetConstruction::EXPLICIT && tmp != NetConstruction::LMS)
      {
        throw BadNetConstruction("Multilevel point sets only work with Sobol, LMS and explicit constructions.");
      }
      return tmp;
   }
};

}}

#endif
