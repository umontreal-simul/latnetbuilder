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

#include "latcommon/ProductWeights.h"
#include "latcommon/UtilLC.h"
#include <sstream>

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;
using boost::bad_lexical_cast;


namespace LatCommon
{

//===========================================================================

ProductWeights::ProductWeights (Weight defaultWeight)
   : m_defaultWeight(defaultWeight)
{
}

//===========================================================================

void ProductWeights::setWeightForCoordinate (unsigned coordinate, Weight weight)
{
   if (coordinate >= m_weights.size())
      m_weights.resize(coordinate + 1);
   m_weights[coordinate] = weight;
}

//===========================================================================

Weight ProductWeights::getWeight (const Coordinates & projection) const
{
   if (projection.empty())
      return m_defaultWeight;

   Weight w = 1.0;
   Coordinates::const_iterator it = projection.begin();
   while (it != projection.end()) {
      w *= getWeightForCoordinate(*it);
      ++it;
   }
   return w;
}

//===========================================================================

void ProductWeights::format(std::ostream& os) const
{
   using LatCommon::operator<<;
   os << "ProductWeights(" << m_weights << ", default=" << m_defaultWeight << ")";
}

} // namespace LatCommon

//===========================================================================

#ifdef WITH_XML
#include "xmlerror.hpp"

namespace LatCommon
{

ProductWeights* ProductWeights::createFromXML (const pugi::xml_node & root)
{
   ProductWeights* o = new ProductWeights();

   pugi::xml_node node;

   try {
      // default weight (optional)
      node = root.child("default").child("weight");
      if (node)
         o->setDefaultWeight(lexical_cast<Weight>(node.child_value()));

      // per-coordinate weights
      for (pugi::xml_node cnode = root.child("coordinate"); cnode; cnode = cnode.next_sibling("coordinate")) {
         // weight
         node = cnode.child("weight");
         if (!node)
            throw pugi::xml_error(cnode, "missing <weight> element");
         Weight weight = lexical_cast<Weight>(node.child_value());
         // coordinate index
         node = cnode.child("coordinate");
         if (!node)
            throw pugi::xml_error(cnode, "missing <coordinate> element");
         int coordinate = lexical_cast<Weight>(node.child_value());
         // store weight
         o->setWeightForCoordinate(coordinate, weight);
      }

      return o;
   }
   catch (bad_lexical_cast& e) {
      delete o;
      throw pugi::xml_error(node, e.what());
   }
}

} // namespace LatCommon
#endif
