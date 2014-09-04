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

#ifndef REDUCER_H
#define REDUCER_H
 
#include "latcommon/Types.h"
#include "latcommon/Const.h"
#include "latcommon/UtilLC.h"
#include "latcommon/Base.h"
#include "latcommon/IntLattice.h"
#include <fstream>
#include <sstream>
#include <vector>


namespace LatCommon {
/**
 * 
 * 
 * For a given lattice, this class implements methods to reduce its basis in the
 * sense of Minkowski and to find the shortest non-zero vector of the lattice
 * using pre-reductions and a branch-and-bound (BB) algorithm \cite rLEC97c .
 * It also implements the method of  Lenstra, Lenstra and Lovasz (LLL) \cite mLEN82a
 * as well as  the method of Dieter \cite rDIE75a to reduce a lattice basis.
 * The reduction algorithms in this class use both the primal and the dual lattices,
 * so both lattices must be defined.
 * 
 * 
 */

class Reducer {
public:

   /**
    * Whenever the number of nodes in the branch-and-bound tree exceeds
    * \c SHORT_DIET in the method \c ShortestVector,
    * \c PreRedDieterSV
    * is automatically set to \c true for the next call;
    * otherwise it is set to \c false.
    * The default value is 1000.
    */
   static const long SHORT_DIET = 1000;

   /**
    * Whenever the number of nodes in the branch-and-bound tree exceeds
    * \c SHORT_LLL in the  method \c ShortestVector,
    * \c PreRedLLLSV is automatically set to \c true for the next call;
    * otherwise it is set to \c false.
    * %   The default value is 1000.
    */
   static const long SHORT_LLL = 1000;

   /**
    * Whenever the number of nodes in the branch-and-bound tree exceeds
    * \c MINK_LLL in the  method \c reductMinkowski,
    * \c PreRedLLLRM is automatically set to \c true for the
    * next call; otherwise it is set to \c false.
    */
   static const long MINK_LLL = 500000;

   /**
    * Maximum number of transformations in the method \c PreRedDieter.
    * After \c MAX_PRE_RED successful transformations have been performed,
    * the prereduction is stopped.
    */
   static const long MAX_PRE_RED = 1000000;

   /**
    * The maximum number of nodes % that we are ready to accept
    * in the branch-and-bound tree when calling \c ShortestVector
    * or \c reductMinkowski.  When this number is exceeded, the method
    * aborts and returns \c false.
    */
   static long maxNodesBB;

   /**
    * These boolean variables indicate which type of pre-reduction
    * is to be performed for \c ShortestVector (SV) and for
    * \c reductMinkowski (RM).
    * \c Dieter means the pairwise pre-reduction as in the method
    * \c PreRedDieter.
    * \c LLL means the LLL reduction of Lenstra, Lenstra, and Lov\'asz.
    * The variable \c PreRedDieterSV is originally set to \c true
    * and the two others are originally set to \c false.
    * These variables are reset automatically depending on the thresholds
    * <tt>MinkLLL, ShortDiet, ShortLLL</tt> as explained above.
    */
   static bool PreRedDieterSV;
   static bool PreRedLLLSV;
   static bool PreRedLLLRM;

   /**
    * Constructor. Initializes the reducer to work on lattice \c lat.
    */
   Reducer (IntLattice & lat);

   /**
    * Copy constructor.
    */
   Reducer (const Reducer & red);

   /**
    * Destructor.
    */
   ~Reducer ();

   /**
    * Assignment operator.
    */
   Reducer & operator= (const Reducer & red);

   /**
    * Copies the reducer \c red into this object. 
 * \todo Richard: Encore utile?
 * 
    */
   void copy (const Reducer & red);

   /**
    * Method used in \c reductMinkowski to perform a transformation
    * of stage 3 described in \cite rAFF85a . Also used in \c ShortestVector.
    * Assumes that \f$\sum_{i=1}^t z_i V_i\f$ is a short vector that will enter
    * the basis.  Tries to reduce some vectors by looking for indices \f$i < j\f$
    * such that \f$|z_j| > 1\f$ and \f$q=\lfloor z_i/z_j\rfloor \not=0\f$,
    * and adding \f$q V_i\f$ to \f$V_j\f$ when this happens.
    * Returns in \f$k\f$ the last index \f$j\f$ such that \f$|z_j|=1\f$.
    */
   void transformStage3 (std::vector<long> & z, int & k);

   /**
    * Finds a Choleski decomposition of the basis. Returns in \c C0
    * the elements of the upper triangular matrix of the Choleski decomposition
    * that are above the diagonal. Returns in \c DC2 the
    * squared elements of the diagonal.
    */
   bool calculCholeski (RVect & DC2, RMat & C0);

   /**
    * Tries to find shorter vectors in \c reductMinkowski.
    */
   bool tryZ  (int j, int i, int Stage, bool & smaller, Base & WTemp);

   /**
    * Tries to find a shorter vector in \c shortestVector.
    */
   bool tryZ0 (int j, bool & smaller);

   /**
    * Computes the shortest non-zero vector of this lattice with respect to norm
    * \c norm using branch-and-bound and the algorithm described in
    * \cite rLEC97c .
    * The \c Norm member of this object  will be changed to \c norm.
    * If \c MaxNodesBB is exceeded during {one} of the branch-and-bounds,
    * the method aborts and returns \c false.
    * Otherwise, it returns \c true. Uses the pre-reduction algorithms
    * of Dieter and of Lenstra-Lenstra-Lovasz.
    */
   bool shortestVector (NormType norm);

   /**
    * Similar to \c ShortestVector but uses the algorithm of
    * Dieter \cite rDIE75a, \cite rKNU98a .
    */
   bool shortestVectorDieter (NormType norm);

   /**
    * Tries to shorten the vectors of the primal basis using branch-and-bound,
    * in \c reductMinkowski.
    */
   bool redBB (int i, int d, int Stage, bool & smaller);

   /**
    * Tries to shorten the smallest vector of the primal basis
    * using branch-and-bound, in \c ShortestVector.
    */
   bool redBB0 (NormType norm);

   /**
    * Performs the reductions of the preceding two methods
    * using cyclically all values of \f$i\f$ (only for \f$i > d\f$ in the latter case)
    * and stops after either \c MaxPreRed successful transformations have been
    * achieved or no further reduction is possible.
    * Always use the Euclidean norm.
    */
   void preRedDieter (int d);

   /**
    * Finds the shortest non-zero vector using norm \c norm.
 * Does not modify the basis.
 * Stops and returns \c false if not finished after examining possib.
    * Returns \c true upon success., and returns length in Lmin.
    * Uses the algorithm of Dieter \cite rDIE75a given in Knuth \cite rKNU98a .
    */
   bool redDieter (NormType norm);

   /**
    * Performs a LLL (Lenstra-Lenstra-Lovasz) basis reduction up to dimension
    * \c dim with coefficient \c fact, which must be smaller than 1.
    * If \c fact is closer to 1, the basis will be
    * (typically) ``more reduced'', but that will require more work.
    * The reduction algorithm is applied until \c maxcpt successful
    * transformations have been done. Always uses the Euclidean norm.
    */
   void redLLL (double fact, long maxcpt, int dim);

   /**
    * Reduces the current basis to a Minkowski reduced basis
    * with respect to the Euclidean norm, assuming
    * that the first \f$d\f$ vectors are already reduced and sorted.
    * If \c MaxNodesBB is exceeded during {one} of the branch-and-bound step,
    * the method aborts and returns \c false.
    * Otherwise it returns \c true, the basis is reduced and
    * sorted by vector lengths (the shortest vector is \c V[1] and
    * the longest is \c V[Dim]).
    * %   This method does not care about numerical imprecision due to the
    * %    (64-bit) floating-point representation.
    * %    In this sense, the results are not 100% reliable.
    */
   bool reductMinkowski (int d);

   /**
    * Performs pairwise reductions.
    * This method tries to reduce each basis vector with
    * index larger than \f$d\f$ and distinct from \f$i\f$ by adding to it a
    * multiple of the \f$i\f$-th vector.
    * Always uses the Euclidean norm.
    */
   void pairwiseRedPrimal (int i, int d);

   /**
    * Performs pairwise reductions, trying to reduce every other vector of
    * the {\em dual\/} basis by adding multiples of the \f$i\f$-th vector.
    * That may change the \f$i\f$-th vector in the primal basis.
    * Each such dual reduction is actually performed only if that does not
    * increase the length of vector \f$i\f$ in the primal basis.
    * Always uses the Euclidean norm.
    */
   void pairwiseRedDual (int i);

   /**
    * Returns the length of the shortest basis vector in the lattice.
    */
   NScal getMinLength ()  {
      if (m_lat->getNorm() == L2NORM)
         return sqrt(m_lMin2);
      else return m_lMin; } 

   /**
    * Sets the lower bound on the square length of the shortest vector in
    * dimension \f$i\f$ to \f$V[i]\f$,  for \f$i\f$ going from \c dim1 to \c dim2.
    */
   void setBoundL2 (const NVect & V, int dim1, int dim2);

   /**
    * Debug function that print the primal and dual bases.
    */
   void trace (char *mess);


/**
 * Lattice on which the reduction will be performed.
 */
private:

   IntLattice* m_lat;

   /**
    * Permutes the \f$i^{th}\f$ and the \f$j^{th}\f$ line, and the \f$i^{th}\f$ and the \f$j^{th}\f$ column
    * of the scalar product's matrix.
    */
   void permuteGramVD (int i, int j, int n);

   /**
    * Recalculates the first \f$n\f$ entries of the \f$j^{th}\f$ column of the Choleski's matrix
    * of order 2.
    */
   void calculCholeski2LLL (int n, int j);

   /**
    * Recalculates the entry (\f$i\f$, \f$j\f$) of the Choleski's matrix of order 2.
    */
   void calculCholeski2Ele (int i, int j);

   /**
    * 
    */
   void miseAJourGramVD (int j);

   /**
    * 
    */
   void calculGramVD ();

   /**
    * 
    */
   void reductionFaible (int i, int j);

   /**
    * Lower bound on the length squared of the shortest vector in each dimension.
    * If any vector of the lattice is shorter than this bound, we stop the reduction
    * immediately and reject this lattice since its shortest vector will be
    * even smaller.
    */
   NVect m_BoundL2;

   /**
    * Work variables.
    */
   BScal m_bs;
   BVect m_bv;            // Saves shorter vector in primal basis
   BVect m_bw;            // Saves shorter vector in dual basis

   NScal m_lMin, m_lMin2;
   NScal m_ns;
   NVect m_nv; 

   RScal m_rs;
   RVect m_zLR, m_n2, m_dc2;
   RMat m_c0, m_c2, m_cho2, m_gramVD;
   int *m_IC;             // Indices in Cholesky 
   
   std::vector<long> m_zLI;
   std::vector<long> m_zShort;
   long m_countNodes;     // Counts number of nodes in the branch-and-bound tree
   long m_countDieter;    // Counts number of attempts since last successful
                          // Dieter transformation
   long m_cpt;            // Counts number of successes in pre-reduction
                          // transformations
   bool m_foundZero;      // = true -> the zero vector has been found
// long m_BoundL2Count;   // Number of cases for which the reduction stops
                          // because any vector is shorter than L2 Bound.
};

}


#endif // REDUCER_H

