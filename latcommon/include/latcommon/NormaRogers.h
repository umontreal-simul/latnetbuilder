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
 
/* NormaRogers.h for ISO C++ */
#ifndef NORMAROGERS_H
#define NORMAROGERS_H

#include "latcommon/Normalizer.h"
#include <stdexcept>


namespace LatCommon {
/**
 * 
 * 
 * This class implements the <em>Rogers</em> bounds on the density of
 * sphere packing. The length of vectors is computed using the
 * \f${\cal L}_2\f$ norm. The bounding lengths, for a lattice of rank \f$k\f$
 * containing \f$m\f$ points per unit volume in dimension \f$t\f$, are given by
 * \f$\ell_t^* = \gamma_t m^{k/t}\f$ for \f$t \ge k\f$, where the \f$\gamma_t\f$ are
 * the <em>Rogers</em> lattice constants.
 * 
 * 
 */
class NormaRogers : public Normalizer {
public:

   /**
    * Constructor for the Rogers bounds. The lattices are those of rank \f$k\f$,
    * with \f$m\f$ points per unit volume, in all dimensions  \f$\le t\f$.
    * The bias factor \c beta \f$= \beta\f$ gives more weight to some of the
    * dimensions. There is no restriction on the dimension \f$t\f$ which can be
    * larger than 48.
    */
   NormaRogers (const MScal & m, int k, int t, double beta = 1);

   /**
    * Destructor.
    */
   ~NormaRogers();

   /**
    * Returns the value of the Rogers lattice constant \f$\gamma_j\f$ in dimension  \f$j\f$.
    */
   double getGamma (int j) const throw (std::out_of_range);


/**
 * The lattice constants \f$\gamma_j\f$ are the Rogers bounds in each dimension \f$j\f$.
 */
private:

   double *m_gamma;

   /**
    * Precomputed lattice constants \f$\gamma_j\f$ for the Rogers bounds in each
    * dimension \f$j \le 48\f$.
    */
   static const double m_gamma0[1 + Normalizer::MAX_DIM];

   /**
    * Computes the Rogers bound in dimension \f$d\f$.
    */
   double calcGamma (int d);

};

}


#endif

