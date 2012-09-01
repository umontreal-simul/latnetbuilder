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

#include "latcommon/OrderDependentWeights.h"
#include <sstream>

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;
using boost::bad_lexical_cast;

namespace LatCommon
{

//===========================================================================

OrderDependentWeights::OrderDependentWeights (Weight defaultWeight)
   : m_defaultWeight(defaultWeight)
{
}

//===========================================================================

void OrderDependentWeights::setWeightForOrder (unsigned order, Weight weight)
{
   if (order >= m_weights.size())
      m_weights.resize(order + 1);
   m_weights[order] = weight;
}

//===========================================================================

Weight OrderDependentWeights::getWeight (const Coordinates& projection) const
{
   return getWeightForOrder(projection.size());
}

//===========================================================================

void OrderDependentWeights::format(std::ostream& os) const
{
   using LatCommon::operator<<;
   os << "OrderDependentWeights(" << m_weights << ", default=" << m_defaultWeight << ")";
}

} // namespace LatCommon

//===========================================================================

#ifdef WITH_XML
#include "xmlerror.hpp"

namespace LatCommon
{

OrderDependentWeights* OrderDependentWeights::createFromXML (const pugi::xml_node& root)
{
   OrderDependentWeights* o = new OrderDependentWeights();

   pugi::xml_node node;

   try {
      // default weight
      node = root.child("default").child("weight");
      if (node)
         o->setDefaultWeight(lexical_cast<Weight>(node.child_value()));

      // per-dimension weights
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
         o->setWeightForOrder(dimension, weight);
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
