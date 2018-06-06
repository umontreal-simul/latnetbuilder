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

#include "netbuilder/Types.h"

namespace LatBuilder { namespace Parser {

template <>
LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::PointSetType::UNILEVEL>
SizeParam<LatticeType::ORDINARY, LatBuilder::PointSetType::UNILEVEL>::parse(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<uInteger, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::PointSetType::UNILEVEL>(n.first);
   else
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::PointSetType::UNILEVEL>(intPow(n.first, n.second));
}

template <>
LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::PointSetType::MULTILEVEL>
SizeParam<LatticeType::ORDINARY, LatBuilder::PointSetType::MULTILEVEL>::parse(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<uInteger, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::PointSetType::MULTILEVEL>(n.first);
   else
      return LatBuilder::SizeParam<LatticeType::ORDINARY, LatBuilder::PointSetType::MULTILEVEL>(n.first, n.second);
}

template <>
LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::UNILEVEL>
SizeParam<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::UNILEVEL>::parse(const std::string& str)
{
   
   auto n = splitPair<std::string, Level>(str, '^', 0);
   std::string str_NTLInput = LatticeParametersParseHelper<LatticeType::POLYNOMIAL>::ToParsableModulus(n.first);
   try {
      Polynomial base = boost::lexical_cast<Polynomial>(str_NTLInput);
      
      // try b^p form first
      
      if (n.second == 0)
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::UNILEVEL>(base);
      else
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::UNILEVEL>(intPow(base, n.second));
   }
   catch (boost::bad_lexical_cast&) {
      throw ParserError("cannot interpret \"" + n.first + "\" as " + TypeInfo<Polynomial>::name());
   }
}

template <>
LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::MULTILEVEL>
SizeParam<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::MULTILEVEL>::parse(const std::string& str)
{
   auto n = splitPair<std::string, Level>(str, '^', 0);
   std::string str_NTLInput = LatticeParametersParseHelper<LatticeType::POLYNOMIAL>::ToParsableModulus(n.first);
   try{

      Polynomial base = boost::lexical_cast<Polynomial>(str_NTLInput);
      // try b^p form first
      
      if (n.second == 0)
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::MULTILEVEL>(base);
      else
         return LatBuilder::SizeParam<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::MULTILEVEL>(base, n.second);
   }
   catch (boost::bad_lexical_cast&) {
      throw ParserError("cannot interpret \"" + n.first + "\" as " + TypeInfo<Polynomial>::name());
   }
}

template <>
LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::PointSetType::UNILEVEL>
SizeParam<LatticeType::DIGITAL, LatBuilder::PointSetType::UNILEVEL>::parse(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<uInteger, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::PointSetType::UNILEVEL>(n.first);
   else
      return LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::PointSetType::UNILEVEL>(intPow(n.first, n.second));
}

template <>
LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::PointSetType::MULTILEVEL>
SizeParam<LatticeType::DIGITAL, LatBuilder::PointSetType::MULTILEVEL>::parse(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<uInteger, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::PointSetType::MULTILEVEL>(n.first);
   else
      return LatBuilder::SizeParam<LatticeType::DIGITAL, LatBuilder::PointSetType::MULTILEVEL>(n.first, n.second);
}

}}
