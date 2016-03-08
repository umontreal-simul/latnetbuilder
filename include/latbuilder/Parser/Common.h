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

#ifndef LATBUILDER__PARSER__COMMON_H
#define LATBUILDER__PARSER__COMMON_H

#include "latbuilder/TypeInfo.h"

#include <memory>
#include <string>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

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

}}

#endif
