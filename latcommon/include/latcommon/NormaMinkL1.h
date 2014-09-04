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
 
/* NormaMinkL1.h for ISO C++ */
#ifndef NORMAMINKL1_H
#define NORMAMINKL1_H

#include "latcommon/Normalizer.h"
#include <stdexcept>


namespace LatCommon {
/**
 * 
 * 
 * This class implements theoretical bounds on the length of the shortest
 * nonzero vector in a lattice, based on the densest sphere packing in
 * space.  The length of vectors is computed using the
 * \f${\cal L}_1\f$ norm. Here,  the length of the shortest nonzero vector
 * gives the minimal number of hyperplanes that cover all the points of
 * the lattice. The following upper bound in this case was established by
 * Marsaglia \cite rMAR68a by  applying the general convex body theorem of
 * Minkowski:
 * \f[
 * \ell_t^* = (t! m^k)^{1/t} = \gamma_t m^{k/t},
 * \f]
 * for  lattices of rank \f$k\f$, containing \f$m\f$ points per unit volume,
 * in dimension \f$t\f$.
 * The lattice constants are thus \f$\gamma_t = (t!)^{1/t}\f$.
 * 
 * 
 * 
 */
class NormaMinkL1 : public Normalizer {
public:

   /**
    * Constructor for the Marsaglia's bounds with the \f${\cal L}_1\f$ norm. The
    * lattices are those of rank \f$k\f$, with \f$m\f$ points per unit volume, in all
    * dimensions  \f$\le t\f$. The bias factor \c beta \f$= \beta\f$ gives more weight
    * to some of the dimensions. Restriction: \f$t \le 48\f$.
    */
   NormaMinkL1 (const MScal & m, int k, int t, double beta = 1);

   /**
    * Returns the value of the lattice constant \f$\gamma_j\f$ in dimension  \f$j\f$.
    */
   double getGamma (int j) const throw (std::out_of_range);


/**
 * Lattice constants \f$\gamma_j\f$ for Marsaglia's bounds in each
 * dimension \f$j\f$.
 */
private:

   static const double m_gamma[1 + Normalizer::MAX_DIM];

};

}

#endif

