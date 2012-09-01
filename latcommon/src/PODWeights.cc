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

#include "latcommon/PODWeights.h"
#include <sstream>

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;
using boost::bad_lexical_cast;

namespace LatCommon
{

//===========================================================================

PODWeights::PODWeights ()
{
}

//===========================================================================

Weight PODWeights::getWeight (const Coordinates& projection) const
{
   return m_orderDependentWeights.getWeight(projection) * m_productWeights.getWeight(projection);
}

//===========================================================================

void PODWeights::format(std::ostream& os) const
{
   using LatCommon::operator<<;
   os << "PODWeights(" << m_orderDependentWeights << ", " << m_productWeights << ")";
}

} // namespace LatCommon

//===========================================================================

#ifdef WITH_XML
#include "xmlerror.hpp"

namespace LatCommon
{

PODWeights* PODWeights::createFromXML (const pugi::xml_node& root)
{
   PODWeights* o = new PODWeights();

   pugi::xml_node node;

   try {
      // order-dependent weights
      node = root.child("order-dependent");
      if (!node)
         throw pugi::xml_error(root, "missing <order-dependent> element");
      OrderDependentWeights* odw = OrderDependentWeights::createFromXML(*node);
      o->m_orderDependentWeights = *odw;
      delete odw;

      // product weights
      node = root.child("product");
      if (!node)
         throw pugi::xml_error(root, "missing <product> element");
      ProductWeights* pw = ProductWeights::createFromXML(*node);
      o->m_productWeights = *pw;
      delete pw;

      return o;
   }
   catch (bad_lexical_cast& e) {
      delete o;
      throw pugi::xml_error(node, e.what());
   }
}

} // namespace LatCommon
#endif
