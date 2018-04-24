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

#include "latbuilder/Parser/SizeParam.h"
#include "latbuilder/Parser/Common.h"
#include "latbuilder/Util.h"

namespace LatBuilder { namespace Parser {

template <>
LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE>
SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE>::parse(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<uInteger, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE>(n.first);
   else
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE>(intPow(n.first, n.second));
}

template <>
LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::EMBEDDED>
SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::EMBEDDED>::parse(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<uInteger, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::EMBEDDED>(n.first);
   else
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::EMBEDDED>(n.first, n.second);
}

template <>
LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::SIMPLE>
SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::SIMPLE>::parse(const std::string& str)
{
   
   auto n = splitPair<std::string, Level>(str, '^', 0);
   std::string str_NTLInput = LatticeParametersParseHelper<LatticeType::POLYNOMIAL>::ToParsableModulus(n.first);
   try {
      Polynomial base = boost::lexical_cast<Polynomial>(str_NTLInput);
      
      // try b^p form first
      
      if (n.second == 0)
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::SIMPLE>(base);
      else
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::SIMPLE>(intPow(base, n.second));
   }
   catch (boost::bad_lexical_cast&) {
      throw ParserError("cannot interpret \"" + n.first + "\" as " + TypeInfo<Polynomial>::name());
   }
}

template <>
LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::EMBEDDED>
SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::EMBEDDED>::parse(const std::string& str)
{
   auto n = splitPair<std::string, Level>(str, '^', 0);
   std::string str_NTLInput = LatticeParametersParseHelper<LatticeType::POLYNOMIAL>::ToParsableModulus(n.first);
   try{

      Polynomial base = boost::lexical_cast<Polynomial>(str_NTLInput);
      // try b^p form first
      
      if (n.second == 0)
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::EMBEDDED>(base);
      else
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::EMBEDDED>(base, n.second);
   }
   catch (boost::bad_lexical_cast&) {
      throw ParserError("cannot interpret \"" + n.first + "\" as " + TypeInfo<Polynomial>::name());
   }
}

/*
template LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE> 
         LatBuilder::Parser::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE>::parse(const std::string& str);

template LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::EMBEDDED> 
         LatBuilder::Parser::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::EMBEDDED>::parse(const std::string& str);

template LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::SIMPLE> 
         LatBuilder::Parser::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::SIMPLE>::parse(const std::string& str);

template LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::EMBEDDED> 
         LatBuilder::Parser::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::EMBEDDED>::parse(const std::string& str);

template struct LatBuilder::Parser::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE>;
template struct LatBuilder::Parser::SizeParam<LatticeType::ORDINARY, LatBuilder::LatEmbed::EMBEDDED>;

template struct LatBuilder::Parser::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::SIMPLE>;
template struct LatBuilder::Parser::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::LatEmbed::EMBEDDED>;
*/
}}
