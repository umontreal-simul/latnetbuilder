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

#ifndef NETBUILDER__PARSER__NET_CONSTRUCTION_PARSER_H
#define NETBUILDER__PARSER__NET_CONSTRUCTION_PARSER_H

#include "latbuilder/Parser/Common.h"
#include "netbuilder/Types.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace NetBuilder { namespace Parser {
namespace lbp = LatBuilder::Parser;
/**
 * Exception thrown when trying to parse an invalid size parameter.
 */
class BadNetConstruction : public lbp::ParserError {
public:
   BadNetConstruction(const std::string& message):
      lbp::ParserError("cannot parse construction parameter string: " + message)
   {}
};

/**
 * Parser for construction parameters.
 */
struct NetConstructionParser {
   typedef std::pair<NetBuilder::NetConstruction,std::string> result_type;

   static result_type parse(const std::string& str)
   {
      if (str == "sobol")
         return result_type(NetBuilder::NetConstruction::SOBOL,std::string());
      else if (str == "explicit")
      {
        throw BadNetConstruction(str+ " not implemented");
        // return result_type(NetBuilder::NetConstruction::EXPLCIT,std::string());
      }
      std::vector<std::string> constructionStrings;
      boost::split(constructionStrings, str, boost::is_any_of(":"));
      if (constructionStrings.size()==2 && constructionStrings[0] == "polynomial")
      {
        return result_type(NetBuilder::NetConstruction::POLYNOMIAL,constructionStrings[1]);
      }
      throw BadNetConstruction(str);
   }
};

}}

#endif
