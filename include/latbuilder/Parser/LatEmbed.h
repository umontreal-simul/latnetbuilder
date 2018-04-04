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

#ifndef LATBUILDER__PARSER__LAT_TYPE_H
#define LATBUILDER__PARSER__LAT_TYPE_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Types.h"

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid size parameter.
 */
class BadLatEmbed : public ParserError {
public:
   BadLatEmbed(const std::string& message):
      ParserError("cannot parse size parameter string: " + message)
   {}
};

/**
 * Parser for size parameters.
 */
struct LatEmbed {
   typedef LatBuilder::LatEmbed result_type;

   static result_type parse(const std::string& str)
   {
      if (str == "false")
         return LatBuilder::LatEmbed::SIMPLE;
      else if (str == "true")
         return LatBuilder::LatEmbed::EMBEDDED;
      throw BadLatEmbed(str);
   }
};

}}

#endif
