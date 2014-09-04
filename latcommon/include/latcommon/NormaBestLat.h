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
 
/* NormaBestLat.h for ISO C++ */
#ifndef NORMABESTLAT_H
#define NORMABESTLAT_H
 
#include "latcommon/Normalizer.h"
#include <stdexcept>


namespace LatCommon {
/**
 * 
 * 
 * This class implements the <em>best</em> theoretical bounds on the length of the
 * shortest nonzero vector in a lattice, based on the densest sphere packing in
 * lattices.  The length of a vector is computed using the
 * \f${\cal L}_2\f$ norm. The bounding lengths for a lattice of rank \f$k\f$,
 * containing \f$m\f$ points per unit volume in dimension \f$t\f$, are given by
 * \f$\ell_t^* = \gamma_t m^{k/t}\f$ for \f$t \ge k\f$, where the \f$\gamma_t\f$ are
 * the lattice constants for the <em>best</em> known lattices \cite mCON99a .
 * 
 * 
 */

class NormaBestLat : public Normalizer {
public:

   /**
    * Constructor for the best bounds obtained for lattices. The
    * lattices are those of rank \f$k\f$,
    * with \f$m\f$ points per unit volume, for all dimensions  \f$\le t\f$.
    * The bias factor \c beta \f$= \beta\f$ gives more weight to some of the
    * dimensions. %: taking \f$\beta < 1\f$ inflates the figure of merit by
    * % \f$(1/\beta)^t\f$, thus weakening the requirements for large \f$t\f$ in a
    * % worst-case figure of merit.
    * Restriction: \f$t \le 48\f$.
    */
   NormaBestLat (const MScal & m, int k, int t, double beta = 1);

   /**
    * Returns the value of the lattice constant \f$\gamma_j\f$ in dimension  \f$j\f$.
    */
   double getGamma (int j) const throw (std::out_of_range);


/**
 * Lattice constants \f$\gamma_j\f$ for the most general lattices in each
 * dimension \f$j\f$.
 */
private:

   static const double m_gamma[1 + Normalizer::MAX_DIM];

};

}


#endif

