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

LatCommon::Weight CombinedWeights::getWeight (const LatCommon::Coordinates& projection) const
{
   LatCommon::Weight sum = 0.0;
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
      LatCommon::Coordinates coords;
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
         // FIXME: have Coordinates parse their own XML
         LatCommon::Coordinates projection;
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
