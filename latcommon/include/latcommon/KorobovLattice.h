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

#ifndef KOROBOVLATTICE_H
#define KOROBOVLATTICE_H

#include "latcommon/Types.h"
#include "latcommon/Const.h"
#include "latcommon/IntLattice.h"


namespace LatCommon {
/**
 * 
 * This class implements lattice bases built from a Korobov lattice rule.
 * For a given \f$a\f$, a Korobov lattice basis is formed as follows:
 * \f[
 * \mathbf{b_1} = (1, a, a^2, \ldots, a^{d-1}),\quad
 * \mathbf{b_2} = (0, n, 0, \ldots, 0),\quad  \ldots,\quad
 * \mathbf{b_d} = (0, \ldots, 0, n).
 * \f]
 * 
 * 
 * \note Pierre: Reprogrammer \c incDim de fa\c con efficace comme dans
 * \c MRGLattice
 * 
 */

class KorobovLattice: public IntLattice {
public:

   /**
    * Constructs a Korobov lattice with \f$n\f$ points, maximal dimension
    * \c maxDim using the norm \c norm.
    */
   KorobovLattice (const MScal & n, const MScal & a, int maxDim,
                   NormType norm = L2NORM);

   /**
    * Constructor. Same as above, except the lattice is formed as follow:
    * \f[
    * \mathbf{b_1} = (a^t, a^{t+1}, a^{t+2}, \ldots, a^{t+d-1}),\qquad
    * \mathbf{b_2} = (0, n, 0, \ldots, 0),\qquad  \ldots,\qquad
    * \mathbf{b_d} = (0, \ldots, 0, n).
    * \f]
    */
   KorobovLattice (const MScal & n, const MScal & a, int dim, int t,
                   NormType norm = L2NORM);

   /**
    * Copy constructor.
    */
   KorobovLattice (const KorobovLattice & Lat);

   /**
    * Assigns \c Lat to this object.
    */
   KorobovLattice & operator= (const KorobovLattice & Lat);

   /**
    * Destructor.
    */
   ~KorobovLattice();

   /**
    * Returns the multiplier \f$a\f$ as a string.
    */
   std::string toStringCoef() const;

   /**
    * Builds the basis in dimension \f$d\f$.
    */
   void buildBasis (int d);

   /**
    * Increments the dimension of the basis by 1.
    */
   void incDim();

   /**
    * Increments the dimension of the basis by 1 by rebuilding the basis
    * from scratch. This is very slow. It can be used for verification of the
    * fast \c incDim method above.
    */
   void incDimSlow();


/**
 * The multiplier of the Korobov lattice rule.
 */
protected:

   MScal m_a;

   /**
    * The shift applied to the lattice rule.
    */
   int m_shift;

   /**
    * Initialization.
    */
   void init();

};

}

#endif

