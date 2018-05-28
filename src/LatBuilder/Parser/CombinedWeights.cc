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

#include "latbuilder/Parser/CombinedWeights.h"
#include "latbuilder/Parser/Weights.h"
#include "latticetester/ProjectionDependentWeights.h"
#include "latticetester/OrderDependentWeights.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace LatBuilder { namespace Parser {

namespace {
   Real inputPowerScale = 1.0;

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
      auto ow = new LatticeTester::OrderDependentWeights;
      auto pw = new LatticeTester::ProjectionDependentWeights;

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
         LatticeTester::Coordinates coords;
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
         LatticeTester::Weight weight;
         is >> weight;
         weight = std::pow(weight, inputPowerScale);

         if (is_default)
            ow->setDefaultWeight(weight);
         else if (is_order)
            ow->setWeightForOrder(order, weight);
         else
            pw->setWeight(coords, weight);

         // skip across-pairs separator
         skip_any(is, sep);
      }
      weights.add(std::unique_ptr<LatticeTester::Weights>(ow));
      weights.add(std::unique_ptr<LatticeTester::Weights>(pw));
      return is;
   }
}

bool
CombinedWeights::parseFile(
      const std::string& arg,
      LatBuilder::CombinedWeights& weights,
      Real powerScale)
{
   Real oldScale = inputPowerScale;
   // set parameter from anonymous namespace
   inputPowerScale = powerScale;
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
   inputPowerScale = oldScale;
   return true;
}

std::unique_ptr<LatBuilder::CombinedWeights>
CombinedWeights::parse(const std::vector<std::string>& args, Real powerScale)
{
   auto w = new LatBuilder::CombinedWeights;
   for (const auto& s : args) {
      if (parseFile(s, *w, powerScale))
         continue;
      w->add(Parser::Weights::parse(s, powerScale));
   }
   return std::unique_ptr<LatBuilder::CombinedWeights>(w);
}

}}
