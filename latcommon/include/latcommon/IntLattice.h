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

#ifndef INTLATTICE_H
#define INTLATTICE_H

#include "latcommon/Types.h"
#include "latcommon/Const.h"
#include "latcommon/Base.h"
#include "latcommon/Normalizer.h"
#include "latcommon/CoordinateSets.h"
#include <string>


namespace LatCommon {
/**
 * 
 * 
 * This class offers tools to manipulate lattice bases.
 * Each lattice is represented by a basis \f$V\f$ and its dual \f$W\f$.
 * It is sometimes possible, as in the case of lattices associated with
 * LCGs (linear congruential generators) or MRGs (multiple recursive generators),
 * to multiply a lattice (and its dual) by a constant factor \f$m\f$
 * in such a way that they are included in \f$\mathbb{Z}^t\f$, allowing
 * exact representation of basis vector coordinates.
 * The duality relation will then read \f$V_i\cdot W_j = m\delta_{ij}\f$
 * for some integer constant {\f$m\f$}.
 * 
 * 
 */

class IntLattice {
public:

   /**
    * Constructor. The modulus is initialized to \f$m\f$, the maximal dimension of the
    * bases is set to \c maxDim, and the order is set to \f$k\f$.
    */
   IntLattice (const MScal & m, int k, int maxDim, NormType norm = L2NORM);

   /**
    * Copy constructor. The maximal dimension of the
    * created basis is set equal to \c Lat's current dimension.
    */
   IntLattice (const IntLattice & Lat);

   /**
    * Assigns \c Lat to this object. The maximal dimension of this
    * basis is set equal to \c Lat's current dimension.
    */
   IntLattice & operator= (const IntLattice & Lat);

   /**
    * Destructor.
    */
   virtual ~IntLattice();

   /**
    * Same as assignment operator above.
    */
   void copy (const IntLattice & lattice);

   /**
    * Returns actual dimension \c Dim.
    */
   int getDim() const  { return m_v.getDim(); } 

   /**
    * Sets actual dimension to \f$d\f$.
    */
   void setDim (int d);

   /**
    * Returns the maximal dimension of the lattice.
    */
   int getMaxDim() const  { return m_v.getMaxDim(); } 

   /**
    * Increases dimension \c Dim by 1.
    */
   virtual void incDim();

   /**
    * Builds the basis for the lattice in dimension \c d.
    */
   virtual void buildBasis (int d);

   /**
    * Returns the norm used by the lattice.
    */
   NormType getNorm() const  { return m_v.getNorm(); } 

   /**
    * Returns the value of the modulus \f$m\f$ of the recurrence (the number of points
    * of the lattice).
    */
   MScal getM() const  { return m_m; } 

   /**
    * Returns the square number of points in the lattice.
    */
   NScal getM2() const  { return m_m2; } 

   /**
    * Returns the order.
    */
   int getOrder() const  { return m_order; } 

   /**
    * Creates and returns the normalizer corresponding to criterion \c norma.
    * In the case of the \f$P_\alpha\f$ test, the argument \c alpha = \f$\alpha\f$.
    * In all other cases, it is unused.
    */
   Normalizer * getNormalizer (NormaType norma, int alpha);

   /**
    * Returns the vector of multipliers (or coefficients) \f$A\f$ as a string.
    */
   virtual std::string toStringCoef() const;

   /**
    * Returns the primal basis \c V.
    */
   Base & getPrimalBasis()  { return m_v; } 

   /**
    * Returns the dual basis \c W.
    */
   Base & getDualBasis()  { return m_w; } 

   /**
    * Computes the logarithm of the normalization factor
    * \c m_lgVolDual2 for the merit  in all dimensions %\f$\le \f$ \c maxDim
    * for the dual lattice if \c dualF is \c true, and for the primal
    * lattice if \c dualF is \c false.
    */
   void calcLgVolDual2 (bool dualF);

   /**
    * 
    */
   double getLgVolDual2 (int i) const  { return m_lgVolDual2[i]; } 

   /**
    * 
    */
   bool getXX (int i) const  { return m_xx[i]; } 

   /**
    * 
    */
   void setXX (bool val, int i)  { m_xx[i] = val; } 

   /**
    * Sorts the basis vectors with indices from \f$d+1\f$ to the dimension of the basis
    * by increasing length. The dual vectors are permuted accordingly.
    * Assumes that the lengths of the corresponding basis vectors
    * % \c VV and \c WW
    * are up to date.
    */
   void sort (int d);

   /**
    * Exchanges vectors \f$i\f$ and \f$j\f$ in the basis and in the dual basis.
    */
   void permute (int i, int j);

   /**
    * Writes this basis in file named \c filename.
    */
   void write (const char *filename) const;

   /**
    * For debugging purposes.
    */
   void trace (char* msg);

   /**
    * Writes this basis on standard output. If \c flag \f$ > 0\f$, the norm of
    * the basis vectors will be recomputed; otherwise not.
    */
   void write (int flag);

   /**
    * Exchanges basis \f$V\f$ and its dual \f$W\f$.
    */
   void dualize();

   /**
    * Checks that the bases satisfy the duality relation
    * \f$V[i]\cdot W[j] = m\,\delta_{ij}\f$. If so, returns \c true, otherwise
    * returns \c false.
    */
   bool checkDuality();

   /**
    * Checks that \c Lat's basis and this basis are equivalent.
    * If so, returns \c true, otherwise returns \c false.
    */
   bool baseEquivalence (IntLattice & Lat);

   /**
    * Builds the basis (and dual basis) of the projection \c proj for this
    * lattice.  The result is placed in the \c lattice lattice. The basis is
    * triangularized to form a proper basis.
    */
   void buildProjection (IntLattice* lattice, const Coordinates & proj);


/**
 * 
 */
protected:

   void init();

   /**
    * Cleans and releases all the memory allocated for this lattice.
    */
   void kill();

   /**
    * The order of the basis.
    */
   int m_order;

   /**
    * Number of points per unit volume (\c m_m) and its square (\c m_m2).
    */
   MScal m_m;
   NScal m_m2;

   /**
    * The logarithm \f$\log_2 (m^2)\f$.
    */
   double m_lgm2;

   /**
    * Primal basis of the lattice.
    */
   Base m_v;

   /**
    * Dual basis of the lattice.
    */
   Base m_w;

   /**
    * 
    */
   double *m_lgVolDual2;

   /**
    * Work variables.
    */
   bool *m_xx;
   MScal m_t1, m_t2, m_t3;
   BMat m_vSI;
   Base m_vTemp;
};

}

#endif

