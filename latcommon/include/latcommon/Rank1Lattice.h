// Copyright (c) 2014 David Munger, Pierre L'Ecuyer, Université de Montréal.  
// Copyright (c) 2012 Richard Simard, Pierre L'Ecuyer, Université de Montréal. 
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
// 
// 
// Converted from tcode to Doxygen in 2014.

#ifndef RANK1LATTICE_H
#define RANK1LATTICE_H

#include "latcommon/Types.h"
#include "latcommon/Const.h"
#include "latcommon/IntLattice.h"


namespace LatCommon {
/**
 * 
 * 
 * \note Pierre (hidden): Il faut pouvoir choisir chaque \f$a_i\f$ ind\'ependamment
 * 
 * 
 * This class implements a general rank 1 lattice basis. For the values
 * \f$a_1, a_2, \ldots, a_{d}\f$ given, the \f$d\f$-dimensional lattice basis is formed
 * as:
 * \f[
 * \mathbf{b_1} = (a_1, a_2, \ldots, a_{d}),\quad \mathbf{b_2} = (0, n, 0, \ldots, 0),\quad  \ldots, \quad \mathbf{b_d} = (0, \ldots, 0, n)
 * \f]
 * Without loss of generality, one may choose \f$a_1 = 1\f$.
 * 
 * 
 * 
 */
class Rank1Lattice: public IntLattice {
public:

   /**
    * Constructor. \f$d\f$ represents the number of multipliers in the array \c a.
    */
   Rank1Lattice (const MScal & n, const MVect & a, int d,
                 NormType norm = L2NORM);

   /**
    * Copy constructor.
    */
   Rank1Lattice (const Rank1Lattice & Lat);

   /**
    * Assigns \c Lat to this object.
    */
   Rank1Lattice & operator= (const Rank1Lattice & Lat);

   /**
    * Destructor.
    */
   ~Rank1Lattice();

   /**
    * Returns the vector of multipliers \f$a\f$ as a string.
    */
   std::string toStringCoef() const;

   /**
    * Builds the basis in dimension \f$d\f$.
    */
   void buildBasis (int d);

   /**
    * Increases the dimension by 1.
    */
   void incDim ();


/**
 * Initializes the rank 1 lattice.
 */
protected:

   void init();

   /**
    * The multipliers of the rank 1 lattice rule.
    */
   MVect m_a;
};

}

#endif

