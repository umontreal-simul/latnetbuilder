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

#include "netbuilder/Util.h"
#include "latbuilder/Parser/SizeParam.h"
#include "latbuilder/Parser/Common.h"

namespace NetBuilder {

Polynomial polynomialParserHelper(const std::string& str)
{
   auto n = LatBuilder::Parser::splitPair<std::string, unsigned int>(str, '^', 0);
   std::string str_NTLInput = LatBuilder::Parser::LatticeParametersParseHelper<LatBuilder::LatticeType::POLYNOMIAL>::ToParsableModulus(n.first);
   try {
      Polynomial base = boost::lexical_cast<Polynomial>(str_NTLInput);      
      if (n.second == 0)
         return base;
      else
         return intPow(base, n.second);
   }
   catch (boost::bad_lexical_cast&) {
      throw LatBuilder::Parser::ParserError("cannot interpret \"" + n.first + "\" as a polynomial");
   }
}
}

