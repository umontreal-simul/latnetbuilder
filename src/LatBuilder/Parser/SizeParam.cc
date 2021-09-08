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
   std::vector<std::string> sizeParamStrings;
   boost::split(sizeParamStrings, str, boost::is_any_of(":"));
   try {
      if (sizeParamStrings.size() == 1) {
         // The input is a polynomial in integer representation.
         auto n = splitPair<uInteger, uInteger>(sizeParamStrings[0], '^', 1);
         Polynomial base = PolynomialFromInt(intPow(n.first, n.second));
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::UNILEVEL>(base);
      }
      else if (sizeParamStrings.size() == 2) {
         // We should get the default polynomial.
         auto n = splitPair<unsigned int, unsigned int>(sizeParamStrings[1], '^', 0);
         if (n.second == 0){
            throw ParserError("\"" + sizeParamStrings[1] + "\" is not a valid size for polynomial point sets.");
         }
         std::string polyString = LatBuilder::getDefaultPolynomial(n.second);
         Polynomial defaultPoly = PolynomialFromInt(std::stoi(polyString));
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::UNILEVEL>(defaultPoly);
      }
      else {
         throw ParserError("cannot interpret \"" + sizeParamStrings[sizeParamStrings.size()-1] + "\" as " + TypeInfo<Polynomial>::name());
      }
   }
   catch (boost::bad_lexical_cast&) {
      throw ParserError("cannot interpret \"" + sizeParamStrings[sizeParamStrings.size()-1] + "\" as " + TypeInfo<Polynomial>::name());
   }
}

template <>
LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::MULTILEVEL>
SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::MULTILEVEL>::parse(const std::string& str)
{
   std::vector<std::string> sizeParamStrings;
   boost::split(sizeParamStrings, str, boost::is_any_of(":"));
   try {
      if (sizeParamStrings.size() == 1) {
         // The input is a polynomial in integer representation.
         auto n = splitPair<uInteger, uInteger>(sizeParamStrings[0], '^', 1);
         Polynomial base = PolynomialFromInt(intPow(n.first, n.second));
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::UNILEVEL>(base);
      }
      else {
         throw ParserError("Default polynomials are not possible for multilevel polynomial point sets.");
      }
   }
   catch (boost::bad_lexical_cast&) {
      throw ParserError("cannot interpret \"" + str + "\" as " + TypeInfo<Polynomial>::name());
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
