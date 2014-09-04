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

#ifndef BASE_H
#define BASE_H

#include "latcommon/Const.h"
#include "latcommon/Types.h"
#include <string>


namespace LatCommon {
 /**
  * 
  * 
  * This class represents a basis for a lattice.
  * To compute the length of vectors, one may use either
  * the \f$L_1\f$, the \f$L_2\f$ or the \f$L_\infty\f$ norms. If one uses
  * the \f$L_2\f$ norm and if \c BScal and \c NScal
  * are of type \c double, then the norm could
  * overflow if the  components of the  basis vectors are larger than
  * \f$\approx 2^{500}\f$. In that case, \c NScal may be chosen as
  * \c RR.  If one uses
  * the \f$L_1\f$ or the \f$L_\infty\f$ norms, \c NScal may be chosen
  * the same as  \c BScal.
  * 
 * 
  */
 
class Base : public BMat {
public:

   /**
    * Constructor. Builds a basis of actual dimension \f$d\f$, maximum dimension
    * \c maxDim and with norm \c norm.
    */
   Base (int d, int maxDim, NormType norm = L2NORM);

   /**
    * Copy constructor.
    */
   Base (const Base &);

   /**
    * Destructor.
    */
   ~Base ();

   /**
    * Assignment operator.
    */
   Base & operator= (const Base &);
   
   /**
    * Cleans and releases all memory used by the basis.
    */
   void kill ();

   /**
    * Swaps this basis with the basis \c b.
    */
   void swap (Base & b);

   /**
    * Exchanges vectors \f$i\f$ and \f$j\f$ in the basis.
    */
   void permute (int i, int j);

   /**
    * Returns the actual dimension of the basis.
    */
   int getDim () const  { return m_dim; } 
   
   /**
    * Resets the actual dimension of the basis to \c d, but does
    * not reserve any new memory.
    */
   void setDim (int d);

   /**
    * Returns the maximal dimension of the basis.
    */
   int getMaxDim () const  { return m_maxDim; } 

   /**
    * Returns the norm used by the basis.
    */
   NormType getNorm () const  { return m_norm; } 

   /**
    * Sets the norm used by the basis.
    */
   void setNorm (NormType norm);

   /**
    * Returns \c true if the \f$i\f$-th vector's norm is not updated and
    * erroneous. Returns \c false otherwise.
    */
   bool isNegativeNorm (int i) const  { return m_negFlag[i]; } 

   /**
    * Sets the dirty flag of the norms to \c flag.
    */
   void setNegativeNorm (bool flag);

   /**
    * Sets the negative flag for the \f$j\f$-th vector to \c flag.
    */
   void setNegativeNorm (bool flag, int j)  { m_negFlag[j] = flag; }

   /**
    * Returns the \f$i\f$-th vector's norm.
    */
   NScal getVecNorm (int i) const  { return m_vecNorm[i]; } 

   /**
    * Sets the \f$i\f$-th vector's norm to \c value. The negative flag for this
    * vector is set to false and no data integrity is made.
    */
   void setVecNorm (NScal & value, int i);

   /**
    * Recalculates the norm of each vector in the basis.
    */
   void updateVecNorm (); 

   /**
    * Same as above, except that the recalculation begins at dimension \c d + 1.
    */
   void updateVecNorm (int d); 

   /**
    * Updates the norm of vector at dimension \c d using the \c L2NORM.
    */
   void updateScalL2Norm (int d); 

   /**
    * Updates the norm of all basis vectors from dimensions \c d1 to
    * \c d2 (inclusive) using the \c L2NORM.
    */
   void updateScalL2Norm (int d1, int d2); 

   /**
    * Writes the basis in a string and returns it.
    */
   std::string toString () const;

   /**
    * Writes the \f$i\f$-th basis vector in a string and returns it.
    */
   std::string toString (int i) const;
   
   /**
    * Writes the basis on standard output.
    */
   void write () const;
 
   /**
    * Writes \f$i\f$-th basis vector on standard output.
    */
   void write (int i) const;


/**
 * Actual dimension of the basis.
 */
protected:

   int m_dim;

   /**
    * Maximum dimension of the basis.
    */
   int m_maxDim;

   /**
    * Norm used to calculate the norm of the vectors.
    */
   NormType m_norm;

   /**
    * The norm of each vector in the basis.
    */
   NVect m_vecNorm;

   /**
    * Indicates whether a vector norm must be recalculated or not.
    */
   bool* m_negFlag;     
};

}

#endif

