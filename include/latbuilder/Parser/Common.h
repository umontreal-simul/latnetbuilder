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

#ifndef LATBUILDER__PARSER__COMMON_H
#define LATBUILDER__PARSER__COMMON_H

#include "latbuilder/TypeInfo.h"
#include "latbuilder/Types.h"

#include <memory>
#include <string>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace LatBuilder { namespace Parser {

/**
 * Base class for parsing errors.
 */
class ParserError : public std::runtime_error {
public:
	ParserError(const std::string& message):
		std::runtime_error(message)
	{}
};

/**
 * Splits two tokens separated by \c sep into a pair of values.
 *
 * \param input      Input string.
 * \param sep        Separator character.
 * \param def        Default value for the second element in the returned pair,
 *                   used when the separator cannot be found in the string.
 */
template <typename T1 = std::string, typename T2 = std::string>
std::pair<T1, T2> splitPair(const std::string& input, char sep, const T2& def = T2())
{
   std::string::size_type sep_pos = input.find(sep);

   auto first = input.substr(0, sep_pos);

   try {

      T1 x1 = boost::lexical_cast<T1>(first);

      if (sep_pos == std::string::npos)
         return std::make_pair(std::move(x1), def);

      auto second = input.substr(sep_pos + 1, std::string::npos);

      try {
         T2 x2 = boost::lexical_cast<T2>(second);
         return std::make_pair(std::move(x1), std::move(x2));
      }
      catch (boost::bad_lexical_cast&) {
         throw ParserError("cannot interpret \"" + second + "\" as " + TypeInfo<T2>::name());
      }
   }
   catch (boost::bad_lexical_cast&) {
      throw ParserError("cannot interpret \"" + first + "\" as " + TypeInfo<T1>::name());
   }
}

/**
 * Splits a list comma-separated values into a vector.
 */
template <typename T = std::string>
std::vector<T> splitCSV(const std::string& input)
{
   std::vector<T> vec;
   boost::tokenizer<boost::escaped_list_separator<char>> tokenizer(input);
   for (const auto& token : tokenizer) {
      try {
         vec.push_back(boost::lexical_cast<T>(token));
      }
      catch (boost::bad_lexical_cast&) {
         throw ParserError("cannot interpret \"" + token + "\" as " + TypeInfo<T>::name());
      }
   }
   return vec;
}

/**
 * Splits a dash-separated list of values into a vector.
 */
template <typename T = std::string>
std::vector<T> splitDash(const std::string& input)
{
   std::vector<T> vec;
   std::vector<std::string> tmp;
   boost::split(tmp, input, boost::is_any_of("-"));
   for (const auto& token : tmp) {
      try {
         vec.push_back(boost::lexical_cast<T>(token));
      }
      catch (boost::bad_lexical_cast&) {
         throw ParserError("cannot interpret \"" + token + "\" as " + TypeInfo<T>::name());
      }
   }
   return vec;
}

/**
*  convert lattice parameter strings to the appropriate input format  
*
*/
template <LatBuilder::LatticeType LR>
struct LatticeParametersParseHelper ;

template <>
struct LatticeParametersParseHelper<LatticeType::ORDINARY> {
   
   static std::string ToParsableModulus (const std::string& str)
   {return str ;}

   
   static std::string ToParsableGenValue (const std::string& str)
   {return str ;}

   static typename LatticeTraits<LatticeType::ORDINARY>::GeneratingVector ParseGeneratingVector(const std::string& str)
   {return splitDash<uInteger>(str);}
};

template <>
struct LatticeParametersParseHelper<LatticeType::POLYNOMIAL> {
   
   static std::string ToParsableModulus (const std::string& str){
      uInteger size = str.size();
      std::string str_NTLInput(2*size -1,' ');
      for(uInteger i = 0; i<size ; i++){
         if (str[i] != '0' && str[i] != '1')
            throw LatBuilder::Parser::ParserError("cannot interpret \"" + str + "\" as a polynomial"); 
         str_NTLInput[2*i] = str[i];
      }
      str_NTLInput = "[" + str_NTLInput + "]" ; 
      return str_NTLInput;
   }

   
   static std::string ToParsableGenValue (const std::string& str){
      uInteger size = str.size();
      std::string str_NTLInput(2*size -1,' ');
      for(uInteger i = 0; i<size ; i++){
          if (str[i] != '0' && str[i] != '1')
            throw LatBuilder::Parser::ParserError("cannot interpret \"" + str + "\" as a polynomial"); 
         str_NTLInput[2*i] = str[i];
      }
      str_NTLInput = "[" + str_NTLInput + "]" ; 
      return str_NTLInput;
   }

   static typename LatticeTraits<LatBuilder::LatticeType::POLYNOMIAL>::GeneratingVector ParseGeneratingVector(const std::string& str)
   {
      auto genVec_str = splitDash<std::string>(str);
      typename LatticeTraits<LatticeType::POLYNOMIAL>::GeneratingVector genVec ;
      for(const auto& gen_str: genVec_str) {
         std::string str_NTLInput = LatticeParametersParseHelper<LatticeType::POLYNOMIAL>::ToParsableGenValue(gen_str);
         genVec.push_back((boost::lexical_cast<typename LatticeTraits<LatticeType::POLYNOMIAL>::GenValue>(str_NTLInput)));
      }
      return genVec;
   }
};

}}

#endif
