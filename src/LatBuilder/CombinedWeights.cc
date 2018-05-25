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

#include "latbuilder/CombinedWeights.h"
#include "latbuilder/TextStream.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

namespace LatBuilder
{

//===========================================================================

CombinedWeights::CombinedWeights ():
   Weights()
{}

//===========================================================================

LatticeTester::Weight CombinedWeights::getWeight (const LatticeTester::Coordinates& projection) const
{
   LatticeTester::Weight sum = 0.0;
   for (const auto& w : m_weights)
      sum += w->getWeight(projection);
   return sum;
}

//===========================================================================

void CombinedWeights::format(ostream& os) const
{
   using TextStream::operator<<;
   os << "CombinedWeights(";
   auto it = m_weights.begin();
   if (it != m_weights.end()) {
      os << **it;
      while (++it != m_weights.end())
         os << ", " << **it;
   }
   os << ")";
}

//===========================================================================
// non-member functions
//===========================================================================

#if 0
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

//===========================================================================

istream& operator>> (istream& is, CombinedWeights& weights)
{
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
               "CombinedWeights from input stream");
      skip_any(is, whitespace);

      // read weight
      Weight weight;
      is >> weight;

      if (is_default)
         weights.setDefaultWeight(weight);
      else if (is_order)
         weights.setDefaultWeightForOrder(order, weight);
      else
         weights.setWeight(coords, weight);

      // skip across-pairs separator
      skip_any(is, sep);
   }
   return is;
}
#endif

} // namespace

//===========================================================================

#if 0
#ifdef WITH_XML
#include "xmlerror.hpp"

namespace LatBuilder
{

CombinedWeights* CombinedWeights::createFromXML (const pugi::xml_node& root)
{
   CombinedWeights* o = new CombinedWeights();

   pugi::xml_node node;

   try {
      // default weight (optional)
      node = root.child("default").child("weight");
      if (node)
         o->setDefaultWeight(lexical_cast<Weight>(node.child_value()));

      // per-dimension weights (optional)
      for (pugi::xml_node dnode = root.child("dimension"); dnode; dnode = dnode.next_sibling("dimension")) {
         // weight
         node = dnode.child("weight");
         if (!node)
            throw pugi::xml_error(dnode, "missing <weight> element");
         Weight weight = lexical_cast<Weight>(node.child_value());
         // dimension (projection order)
         node = dnode.child("dimension");
         if (!node)
            throw pugi::xml_error(dnode, "missing <dimension> element");
         int dimension = lexical_cast<int>(node.child_value());

         // store weight
         o->setDefaultWeightForOrder(dimension, weight);
      }

      // per-projection weights
      for (pugi::xml_node pnode = root.child("projection"); pnode; pnode = pnode.next_sibling("projection")) {

         // weight
         node = pnode.child("weight");
         if (!node)
            throw pugi::xml_error(pnode, "missing <weight> element");
         Weight weight;
         weight = lexical_cast<Weight>(node.child_value());

         // coordinates (projection)
         node = pnode.child("coordinates");
         if (!node)
            throw pugi::xml_error(pnode, "missing <coordinates> element");

         // parse coordinate list as projection
         LatticeTester::Coordinates projection;
         for (node = node.child("coordinate"); node; node = node.next_sibling("coordinate"))
            projection.insert(lexical_cast<int>(node.child_value()));

         // store projection weight
         o->setWeight(projection, weight);
      }

      return o;
   }
   catch (bad_lexical_cast& e) {
      delete o;
      throw pugi::xml_error(node, e.what());
   }
}

} // namespace
#endif
#endif
