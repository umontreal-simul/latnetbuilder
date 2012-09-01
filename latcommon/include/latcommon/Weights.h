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

#ifndef LATCOMMON__WEIGHTS_H
#define LATCOMMON__WEIGHTS_H

#include <string>
#include "latcommon/CoordinateSets.h"


namespace LatCommon {

/**
 * Scalar weight type.
 *
 * \note We could have used \c Weight, but it might be wise to leave this \c
 * typedef in case we decide to use <tt>long Weight</tt> at some point.
 */
typedef double Weight;

/**
 * Abstract weights class.
 *
 * This abstract class is the basis for different kinds of weights used to
 * accentuate the importance given to some projections when computing
 * figures of merit for lattices or point sets.
 */
class Weights {
public:

   /**
	* Destructor.
	*/
   virtual ~Weights()
   { } 

   /**
    * Returns the weight of the projection specified by \c projection.
    */
   virtual Weight getWeight (const Coordinates & projection) const = 0;

protected:
   /**
    * Identifies the type of weights, formats them and outputs them on \c os.
    *
    * \remark Deriving classes should identify themselves in the output.
    */
   virtual void format(std::ostream& os) const = 0;

   friend std::ostream & operator<< (std::ostream & out, const Weights & o);
};


/**
 * \relates Weights
 * Identifies the type of weights, formats them and outputs them on \c os.
 */
inline std::ostream & operator<< (std::ostream & os, const Weights & o)
{ o.format(os); return os; } 

}

#endif
