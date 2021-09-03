// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#ifndef LATBUILDER__PARSER__LATTICE_H
#define LATBUILDER__PARSER__LATTICE_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Types.h"

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid lattce type.
 */
class BadLattice : public ParserError {
public:
   BadLattice(const std::string& message):
      ParserError("cannot parse size parameter string: " + message)
   {}
};

/**
 * Parser Lattice (ORDINARY - POLYNOMIAL).
 */
struct LatticeParser {
   typedef LatBuilder::LatticeType result_type;

   static result_type parse(const std::string& str)
   {
      if (str == "ordinary")
         return LatBuilder::LatticeType::ORDINARY;
      else if (str == "polynomial")
         return LatBuilder::LatticeType::POLYNOMIAL;
      throw BadLattice(str);
   }
};

}}

#endif
