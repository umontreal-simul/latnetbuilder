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

/* Normalizer.h for ISO C++ */
#ifndef NORMALIZER_H
#define NORMALIZER_H
 
#include "latcommon/Types.h"
#include "latcommon/UtilLC.h"
#include "latcommon/Const.h"
#include <string>


namespace LatCommon {
          /**
           * 
           * 
           * Classes which inherit from this base class are used in implementing bounds
           * on the length of the shortest nonzero vector in a lattice \cite mCON99a .
           * These bounds are used to normalize the length of the shortest vectors.
           * Tight lower bounds are available for all dimensions for many important cases.
           * In most cases, the \f${\cal L}_2\f$ norm is used to compute the length of vectors.
           * 
           * For some figures of merit, no useful bounds are known to normalize the length
           * of the shortest vector. In these cases, this base class will be used as
           * normalizer since it simply sets all normalization constants to 1. This is
           * necessary because the tests compare the normalized values of the merit
           * when searching for good lattices.
           * 
           * 
           * 
 * 
           */
class Normalizer {
public:

   /**
    * Constructor for the bounds. Deals with lattices of rank \f$k\f$, having
    * \f$m\f$ points per unit volume, in all dimensions  \f$\le t\f$. \c Name
    * is the name of the Normalizer.
    * The bias factor \c beta \f$= \beta\f$ gives more weight to some of the
    * dimensions: taking \f$\beta < 1\f$ inflates the figure of merit by
    * \f$(1/\beta)^t\f$, thus weakening the requirements for large \f$t\f$ in a
    * worst-case figure of merit.
    * One normally uses \f$\beta = 1\f$.
    *
    * \note Richard: Je crois que ce facteur
    * \c beta devrait disparaître car des poids beaucoup plus
    * généraux sont maintenant implantées dans les classes  \c *Weights.
    */
   static const int MAX_DIM = 48;

   Normalizer (const MScal & m, int k, int t, std::string Name,
               NormType norm = L2NORM, double beta = 1);

   /**
    * Destructor.
    */
   virtual ~Normalizer ()  { delete [] m_cst; }  

   /**
    * Initializes the bounds on the length of the shortest vector. The lattices
    * have \f$m\f$ points per unit volume, are of rank \f$k\f$, and the bias factor is
    * \c beta for all dimensions \f$j \le \f$ \c maxDim.
    */
   void init (const MScal & m, int k, double beta);

   /**
    * Returns this object as a string.
    */
   std::string ToString () const;

   /**
    * Returns the norm associated with this object.
    */
   NormType getNorm () const  { return m_norm; } 

   /**
    * Sets the norm associated with this object to \c norm.
    */
   void setNorm (NormType norm)  { m_norm = norm; } 

   /**
    * Returns the maximal dimension for this object.
    */
   int getDim () const  { return m_maxDim; }  

   /**
    * Returns the bound on the length of the shortest nonzero vector in
    * dimension \f$j\f$.
    * % Indices \f$j\f$ goes from 1 to the value returned by \c dim.
    */
   double & getCst (int j);

   /**
    * Returns the value of the lattice constant \f$\gamma_j\f$ in dimension  \f$j\f$.
    * For this base class, always returns 1.
    */
   virtual double getGamma (int j) const;


/**
 * Name of the normalizer.
 */
protected:

   std::string m_name;

   /**
    * Norm associated with this object.
    */
   NormType m_norm;

   /**
    * Number of points of the lattice per unit volume.
    */
   MScal m_m;

   /**
    * Rank of the lattice.
    */
   int m_rank;

   /**
    * Only elements 1 to \c m_maxDim (inclusive) of arrays are defined.
    */
   int m_maxDim;

   /**
    * Beta factor.
    */
   double m_beta;

   /**
    * Contains the bounds on the length of the shortest nonzero
    * vector in the lattice in each dimension.
    */
   double *m_cst;


/**
 * Use of the copy-constructor and of assigment are forbidden.
 */
private:

   Normalizer (const Normalizer &);
   Normalizer & operator= (const Normalizer &);

};

}


#endif

