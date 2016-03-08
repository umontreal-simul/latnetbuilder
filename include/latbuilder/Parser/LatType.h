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

#ifndef LATBUILDER__PARSER__LAT_TYPE_H
#define LATBUILDER__PARSER__LAT_TYPE_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Types.h"

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid size parameter.
 */
class BadLatType : public ParserError {
public:
   BadLatType(const std::string& message):
      ParserError("cannot parse size parameter string: " + message)
   {}
};

/**
 * Parser for size parameters.
 */
struct LatType {
   typedef LatBuilder::LatType result_type;

   static result_type parse(const std::string& str)
   {
      if (str == "ordinary")
         return LatBuilder::LatType::ORDINARY;
      else if (str == "embedded")
         return LatBuilder::LatType::EMBEDDED;
      throw BadLatType(str);
   }
};

}}

#endif
