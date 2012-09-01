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

#ifndef LATCOMMON__POD_WEIGHTS_H
#define LATCOMMON__POD_WEIGHTS_H

#include "latcommon/Weights.h"
#include "latcommon/OrderDependentWeights.h"
#include "latcommon/ProductWeights.h"
#include <vector>
#ifdef WITH_XML
#include <pugixml.hpp>
#endif


namespace LatCommon {
/**
 * Product and order-dependent (POD) weights.
 *
 * This class implements POD weights.
 * The weight of a projection depends on the order of the projection and on the
 * coordinates that are in the projection.
 */
class PODWeights : public Weights {
protected:

   OrderDependentWeights m_orderDependentWeights;
   ProductWeights m_productWeights;

public:

   /**
    * Constructs order-dependent weights with default weight.
    */
   PODWeights();

   /**
    * Destructor.
    */
   virtual ~PODWeights()
   { } 

   /**
    * Returns the weight of the projection specified by \c projection.
    */
   virtual Weight getWeight (const Coordinates & projection) const;

   /**
    * Returns the order-dependent part of the weights.
    */
   OrderDependentWeights& getOrderDependentWeights()
   { return m_orderDependentWeights; }

   const OrderDependentWeights& getOrderDependentWeights() const
   { return m_orderDependentWeights; }

   /**
    * Returns the product part of the weights.
    */
   ProductWeights& getProductWeights()
   { return m_productWeights; }

   const ProductWeights& getProductWeights() const
   { return m_productWeights; }

#ifdef WITH_XML
   /**
    * Static factory method; create a \c PODWeights object by
    * parsing XML data.
    */
   static PODWeights* createFromXML (const pugi::xml_node & node);
#endif

protected:
   /// \copydoc LatCommon::Weights::format()
   virtual void format(std::ostream& os) const;
};

}

#endif
