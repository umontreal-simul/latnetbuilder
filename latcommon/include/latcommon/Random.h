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

#ifndef RANDOM_H
#define RANDOM_H


namespace LatCommon {
/**
 * 
 * 
 * This class generates random numbers (in fact pseudo-random numbers).
 * The generator used is the 64-bits generator \c LFSR258
 * from L'Ecuyer \cite rLEC99a  with period length near \f$2^{258}\f$
 * for 64-bits machines, and the 32-bits generator \c LFSR113
 * from L'Ecuyer \cite rLEC99a  with period length near \f$2^{113}\f$
 * on  32-bits machines. Thus the random numbers generated will be
 * different on 32-bits and 64-bits machines.
 * 
 * 
 */
class Random {
public:

   /**
    * Constructor using a default seed for the generator.
    * One may reset the seed by calling \c setSeed.
    */
   Random();

   /**
    * Destructor.
    */
   ~Random()  {} 

   /**
    * Returns a random number in \f$[0, 1)\f$. The number has 53 random bits
    * of resolution on 64-bits machines, and 32 random bits
    * on 32-bits machines.
    */
   double randU01();

   /**
    * Return a random integer in \f$[i, j]\f$. The numbers \f$i\f$ and \f$j\f$ can occur.
    * Restriction: \f$i < j\f$.
    */
   int randInt (int i, int j);

   /**
    * Returns random blocks of \f$s\f$ bits (\f$s\f$-bit integers).
    */
   unsigned long randBits (int s);

   /**
    * Sets the seed of the generator. If not called, a default seed is used.
    */
   void setSeed (unsigned long seed);


private:

   unsigned long randValue();

   unsigned long etat1, etat2, etat3, etat4, etat5;
};

}

#endif

