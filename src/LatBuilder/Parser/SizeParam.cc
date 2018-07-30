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

#include "latbuilder/Parser/SizeParam.h"
#include "latbuilder/Parser/Common.h"
#include "latbuilder/Util.h"

#include <fstream>

#include "netbuilder/Types.h"

namespace LatBuilder { namespace Parser {

template <>
LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::EmbeddingType::UNILEVEL>
SizeParam<LatticeType::ORDINARY, LatBuilder::EmbeddingType::UNILEVEL>::parse(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<uInteger, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::EmbeddingType::UNILEVEL>(n.first);
   else
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::EmbeddingType::UNILEVEL>(intPow(n.first, n.second));
}

template <>
LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::EmbeddingType::MULTILEVEL>
SizeParam<LatticeType::ORDINARY, LatBuilder::EmbeddingType::MULTILEVEL>::parse(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<uInteger, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::EmbeddingType::MULTILEVEL>(n.first);
   else
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::EmbeddingType::MULTILEVEL>(n.first, n.second);
}

template <>
LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::UNILEVEL>
SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::UNILEVEL>::parse(const std::string& str)
{
   
   auto n = splitPair<std::string, Level>(str, '^', 0);
   try {
      if (n.first == "2")
      {
            unsigned int degree = (unsigned int) n.second;
            std::string polyString = LatBuilder::getDefaultPolynomial(degree);
            std::string str_NTLInput = LatticeParametersParseHelper<LatticeType::POLYNOMIAL>::ToParsableModulus(polyString);
            Polynomial defaultPoly = boost::lexical_cast<Polynomial>(str_NTLInput);
            return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::UNILEVEL>(defaultPoly);
      }
      std::string str_NTLInput = LatticeParametersParseHelper<LatticeType::POLYNOMIAL>::ToParsableModulus(n.first);
      Polynomial base = boost::lexical_cast<Polynomial>(str_NTLInput);
      // try b^p form first
      
      if (n.second == 0)
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::UNILEVEL>(base);
      else
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::UNILEVEL>(intPow(base, n.second));
   }
   catch (boost::bad_lexical_cast&) {
      throw ParserError("cannot interpret \"" + n.first + "\" as " + TypeInfo<Polynomial>::name());
   }
}

template <>
LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::MULTILEVEL>
SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::MULTILEVEL>::parse(const std::string& str)
{
   auto n = splitPair<std::string, Level>(str, '^', 0);
   std::string str_NTLInput = LatticeParametersParseHelper<LatticeType::POLYNOMIAL>::ToParsableModulus(n.first);
   try{

      Polynomial base = boost::lexical_cast<Polynomial>(str_NTLInput);
      // try b^p form first
      
      if (n.second == 0)
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::MULTILEVEL>(base);
      else
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::MULTILEVEL>(base, n.second);
   }
   catch (boost::bad_lexical_cast&) {
      throw ParserError("cannot interpret \"" + n.first + "\" as " + TypeInfo<Polynomial>::name());
   }
}

template <>
LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::EmbeddingType::UNILEVEL>
SizeParam<LatticeType::DIGITAL, LatBuilder::EmbeddingType::UNILEVEL>::parse(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<uInteger, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::EmbeddingType::UNILEVEL>(n.first);
   else
      return LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::EmbeddingType::UNILEVEL>(intPow(n.first, n.second));
}

template <>
LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::EmbeddingType::MULTILEVEL>
SizeParam<LatticeType::DIGITAL, LatBuilder::EmbeddingType::MULTILEVEL>::parse(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<uInteger, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::EmbeddingType::MULTILEVEL>(n.first);
   else
      return LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::EmbeddingType::MULTILEVEL>(n.first, n.second);
}

}}
