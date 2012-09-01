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

#include "latbuilder/Parser/CombinedWeights.h"
#include "latbuilder/Parser/Weights.h"
#include "latcommon/ProjectionDependentWeights.h"
#include "latcommon/OrderDependentWeights.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace LatBuilder { namespace Parser {

namespace {
   /**
    * Helper function to check the next characters from an input stream.
    *
    * Returns \c true if the next characters in \c is are \c token.  Upon a match, the characters
    * are removed from \c is; otherwise, they are left in \c is.
    */
   static bool check_next_chars(istream& is, const string& token)
   {
      string::const_iterator it = token.begin();
      while (it != token.end()) {
             int c = is.get();
             if (c != *it) {
                    is.putback(c);
                    break;
             }
             ++it;
      }
      if (it == token.end())
             return true;
      while (it != token.begin())
             is.putback(*(--it));
      return false;
   }

   /**
    * Helper function to skip all characters of a given class.
    */
   static void skip_any(istream& is, const string& characters)
   {
      while (characters.find(is.peek()) != string::npos)
         is.get();
   }

   istream& operator>> (istream& is, LatBuilder::CombinedWeights& weights)
   {
      auto ow = new LatCommon::OrderDependentWeights;
      auto pw = new LatCommon::ProjectionDependentWeights;

      string whitespace = " \t\n\r";
      string sep = whitespace + ",";

      // skip leading whitespace
      skip_any(is, whitespace);

      // check if coordinate set is enclosed in braces
      bool with_braces = false;
      if (is.peek() == '{') {
         is.get();
         with_braces = true;
         skip_any(is, whitespace);
      }

      while (is.good()) {

         // exit on closing brace
         if (with_braces && is.peek() == '}') {
            is.get();
            break;
         }

         // skip comments
         if (is.peek() == '#') {
            while (is.get() != '\n');
            skip_any(is, whitespace);
            continue;
         }

         // check if setting default weight
         bool is_default = check_next_chars(is, "default");

         // check if setting default weight for a specific order
         bool is_order = check_next_chars(is, "order");
         unsigned int order = 0;
         if (is_order) {
            skip_any(is, whitespace);
            is >> order;
         }

         // read coordinates
         LatCommon::Coordinates coords;
         if (!(is_default || is_order))
            is >> coords;

         // check for pair separator
         skip_any(is, whitespace);
         if (!(check_next_chars(is, ":") ||
               check_next_chars(is, "->") ||
               check_next_chars(is, "=>")))
            throw invalid_argument("invalid characters while parsing "
                  "ProjectionDependentWeights from input stream");
         skip_any(is, whitespace);

         // read weight
         LatCommon::Weight weight;
         is >> weight;

         if (is_default)
            ow->setDefaultWeight(weight);
         else if (is_order)
            ow->setWeightForOrder(order, weight);
         else
            pw->setWeight(coords, weight);

         // skip across-pairs separator
         skip_any(is, sep);
      }
      weights.add(std::unique_ptr<LatCommon::Weights>(ow));
      weights.add(std::unique_ptr<LatCommon::Weights>(pw));
      return is;
   }
}

bool
CombinedWeights::parseFile(
      const std::string& arg,
      LatBuilder::CombinedWeights& weights)
{
   auto ka = splitPair<>(arg, ':');
   if (ka.first != "file") return false;
   if (ka.second == "-")
      std::cin >> weights;
   else {
      std::ifstream is(ka.second.c_str());
      if (not is.is_open()) throw ParserError("cannot open weights file");
      is >> weights;
      is.close();
   }
   return true;
}

std::unique_ptr<LatBuilder::CombinedWeights>
CombinedWeights::parse(const std::vector<std::string>& args)
{
   auto w = new LatBuilder::CombinedWeights;
   for (const auto& s : args) {
      if (parseFile(s, *w))
         continue;
      w->add(Parser::Weights::parse(s));
   }
   return std::unique_ptr<LatBuilder::CombinedWeights>(w);
}

}}
