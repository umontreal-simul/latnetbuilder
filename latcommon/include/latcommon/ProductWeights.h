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

#ifndef LATCOMMON__PRODUCT_WEIGHTS_H
#define LATCOMMON__PRODUCT_WEIGHTS_H

#include "latcommon/Weights.h"
#include <vector>
#ifdef WITH_XML
#include <pugixml.hpp>
#endif


namespace LatCommon {
/**
 * Product weights.
 *
 * The weight of a projection is equal to the product of the individual weights
 * of each coordinate involved.
 */
class ProductWeights : public Weights {
protected:

   Weight m_defaultWeight;
   std::vector<Weight> m_weights;

public:

   /**
    * Constructs projection-dependent weights with a default weight.
    *
    * \param defaultWeight    Default weight.
    */
   explicit ProductWeights (Weight defaultWeight = 0.0);

   /**
    * Destructor.
    */
   virtual ~ProductWeights()  {} 

   /**
    * Returns the weight of the projection specified by \c projection.
    */
   virtual Weight getWeight (const Coordinates & projection) const;

   /**
    * Returns the weight associated to the given coordinate.
    */
   virtual Weight getWeightForCoordinate (unsigned int coordinate) const  {
      return coordinate < m_weights.size() ? m_weights[coordinate] : m_defaultWeight; } 

   /**
    * Sets the weight for the coordinate specified by \c coordinate.
    */
   virtual void setWeightForCoordinate (unsigned int coordinate, Weight weight);

   /**
    * Sets the default weight of all coordinates for which a weight
    * has not been set explicitly set using #setWeightForCoordinate().
    */
   virtual void setDefaultWeight (Weight weight) 
   { m_defaultWeight = weight; }

#ifdef WITH_XML
   /**
    * Static factory method; create a \c ProductWeights object by
    * parsing XML data.
    */
   static ProductWeights* createFromXML (const pugi::xml_node & node);
#endif

protected:
   /// \copydoc LatCommon::Weights::format()
   virtual void format(std::ostream& os) const;
};

}

#endif
