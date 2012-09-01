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

#ifndef LATCOMMON__UNIFORM_WEIGHTS_H
#define LATCOMMON__UNIFORM_WEIGHTS_H

#include "latcommon/Weights.h"
#include <sstream>
#include <vector>


namespace LatCommon {
/**
 * This class is used to implement the same weight for all projections.
 */
class UniformWeights : public Weights {
protected:

   Weight m_weight;

public:

   /**
    * Constructs uniform weights.
    *
    * \param weight     Weight for all projections.
    */
   explicit UniformWeights (Weight weight)
   { m_weight = weight; } 

   /**
    * Destructor.
    */
   virtual ~UniformWeights()
   { } 

   /**
    * Returns the same weight regardless of the specified indices.
    */
   virtual Weight getWeight (const Coordinates &) const  { return m_weight; } 

protected:
   /// \copydoc LatCommon::Weights::format()
   virtual void format(std::ostream& os) const {
      os << "UniformWeights(" << m_weight << ")";
   }
};

}

#endif

