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

#ifndef LATCOMMON__ORDER_DEPENDENT_WEIGHTS_H
#define LATCOMMON__ORDER_DEPENDENT_WEIGHTS_H

#include "latcommon/Weights.h"
#include <vector>
#ifdef WITH_XML
#include <pugixml.hpp>
#endif


namespace LatCommon {
/**
 * Order-dependent weights.
 *
 * This class implements order dependent  weights.
 * The weight of a projection depends only on the order of the projection,
 * i.e. the dimension of the projection.
 */
class OrderDependentWeights : public Weights {
protected:

   Weight m_defaultWeight;
   std::vector<Weight> m_weights;

public:

   /**
    * Constructs order-dependent weights with default weight.
    *
    * \param defaultWeight   Default weight.
    */
   explicit OrderDependentWeights (Weight defaultWeight = 0.0);

   /**
    * Destructor.
    */
   virtual ~OrderDependentWeights()
   { } 

   /**
    * Returns the weight of the projection specified by \c projection.
    */
   virtual Weight getWeight (const Coordinates & projection) const;

   /**
    * Returns the weight associated to the given order.
    */
   virtual Weight getWeightForOrder (Coordinates::size_type order) const
   { return order < m_weights.size() ? m_weights[order] : m_defaultWeight; } 

   /**
    * Sets the weight for the order specified by \c order.
    */
   virtual void setWeightForOrder (Coordinates::size_type order, Weight weight);

   /**
    * Sets the default weight of all orders for which a weight
    * has not been set explicitly set using #setWeightForOrder().
    */
   virtual void setDefaultWeight (Weight weight) 
   { m_defaultWeight = weight; }

#ifdef WITH_XML
   /**
    * Static factory method; create a \c OrderDependentWeights object by
    * parsing XML data.
    */
   static OrderDependentWeights* createFromXML (const pugi::xml_node & node);
#endif

protected:
   /// \copydoc LatCommon::Weights::format()
   virtual void format(std::ostream& os) const;
};

}

#endif
