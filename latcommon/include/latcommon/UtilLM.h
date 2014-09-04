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

/**
 * 
 * 
 * This module describes various useful functions as well as
 * functions interfacing with NTL.
 * 
 * 
 */

#ifndef UTILLM_H
#define UTILLM_H

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "NTL/tools.h"
#include "NTL/ZZ.h"
#include "latcommon/vec_lzz.h"
#include "latcommon/Types.h"
#include "latcommon/Const.h"



namespace LatMRG {

/**
 * Maximum integer that can be represented exactly as a
 * \c double: \f$2^{53}\f$.
 */
const double MAX_LONG_DOUBLE = 9007199254740992.0;

/**
 * \name NTL compatibility utilities
 * @{
 */

/**
 * Returns 1 if \f$x\f$ is odd, and 0 otherwise.
 */
inline long IsOdd (const long & x)  {
    return x & 1;
}
/**
 * @}
 * \name Mathematical functions
 * @{
 */

/**
 * Returns \f$p^i\f$.
 */
inline long power (long p, long i) 
{
   return NTL::power_long (p, i);
}



/**
 * Sets \f$z = 2^i\f$.
 */
inline void power2 (long & z, long i) 
{
   z = NTL::power_long (2, i);
}



/**
 * Returns \f$\sqrt{x}\f$ for \f$x\ge 0\f$, and \f$-1\f$ for \f$x < 0\f$.
 */
inline double mysqrt (double x) 
{
    if (x < 0.0)
       return -1.0;
    return sqrt (x);
}



/**
 * Returns \f$\sqrt{x}\f$. 
 * \todo Richard: Cette fonction est-elle encore utilis\'ee?
 * 
 */
inline double SqrRoot (double x) 
{
    return sqrt (x);
}



/**
 * Logarithm of \f$x\f$ in base 2.
 */
template <typename T>
inline double Log2 (const T & x)
{
   return log (x) / 0.6931471805599453094;
}



/**
 * Logarithm of \f$x\f$ in base 2.
 */
inline double Log2 (long x)
{
   return log ((double)x) / 0.6931471805599453094;
}



/**
 * Returns 1, 0 or \f$-1\f$ depending on whether \f$x> 0\f$, \f$x= 0\f$ or \f$x< 0\f$
 * respectively.%
 * 
 */
template <typename T>
inline long sign (const T & x)
{
    if (x > 0) return 1; else if (x < 0) return -1; else return 0;
}

/**
 * @}
 * \name Division and remainder
 * @{
 * 
 * For negative operands, the \c / and \c % operators do not give the
 * same results for NTL large integers \c ZZ and for primitive types
 * \c int and \c long. The negative quotient differs by 1 and the
 * remainder also differs.
 * Thus the following small \c inline functions for division and remainder.
 *
 * \todo Richard: Pour certaines fonctions, les résultats sont mis dans les premiers
 * arguments de la fonction pour être compatible avec NTL; pour d'autres,
 * ils sont mis dans les derniers arguments pour être compatible avec notre
 * ancienne
 * version de LatMRG en Modula-2.
 * Plutôt détestable. Je crois qu'il faudra un jour réarranger les arguments
 * des fonctions pour qu'elles suivent toutes la même convention que NTL.
 */


/**
 * Integer division: \f$a = b/d\f$.
 */
inline void div (long & a, const long & b, const long & d)
{
    a = b/d;
}

/**
 * @}
 * \name Vectors
 * @{
 */

/**
 * Allocates memory for the vector \f$A\f$ of dimensions
 * \f$d+1\f$ and initializes its elements to 0.
 */
template <typename Real>
inline void CreateVect (Real* & A, int d)
{
    A = new Real[1 + d];
    for (int i = 0; i <= d; i++)
        A[i] = 0;
}



/**
 * Frees the memory used by the vector \f$A\f$.
 */
template <typename Real>
inline void DeleteVect (Real* & A)
{
    delete[] A;
//    A = 0;
}



/**
 * Creates the vector \f$A\f$ of dimensions \f$d+1\f$
 * and initializes its elements to 0.
 */
template <typename Vect>
inline void CreateVect (Vect & A, int d)
{
    A.SetLength (1 + d);
    // aaa clear (A);
}



/**
 * Frees the memory used by the vector \f$A\f$.
 */
template <typename Vect>
inline void DeleteVect (Vect & A)
{
    A.kill ();
}



/**
 * Sets components \f$[0..d]\f$ of \f$A\f$ to 0.
 */
template <typename Real>
inline void SetZero (Real* A, int d)
{
    for (int i = 0; i <= d; i++)
        A[i] = 0;
}



/**
 * Sets all components \f$[0..d]\f$ of \f$A\f$ to the value \f$x\f$.
 */
template <typename Real>
inline void SetValue (Real* A, int d, const Real & x)
{
    for (int i = 0; i <= d; i++)
        A[i] = x;
}



/**
 * Transforms the polynomial \f$A_0 + A_1x^1 + \cdots + A_nx^n\f$ into
 * \f$x^n - A_1x^{n-1} - \cdots - A_n\f$. The result is put in \f$B\f$.
 */
inline void Invert (const MVect & A, MVect & B, int n)
{
    conv(B[n], 1);
    for(int i = 0; i < n; i++){
       B[i] = -A[n - i];
    }
}



/**
 * Copies vector \f$A\f$ into vector \f$B\f$ using components \f$[0..n]\f$.
 */
template <typename Vect>
inline void CopyVect (const Vect & A, Vect & B, int n)
{
    for (int k = 0; k <= n; k++)  B[k] = A[k];
}



/**
 * Adds vector \f$B\f$ multiplied by \f$x\f$ to vector \f$A\f$ using components
 * \f$[1..n]\f$, and puts the result in \f$A\f$.
 */
template <typename Xcal, typename Scal>
inline void ModifVect (Xcal *A, const Xcal *B, Scal x, int n)
{
    Xcal a;
    conv (a, x);
    for (int i = 1; i <= n; i++)
        A[i] += B[i]*a;
}

/**
 * @}
 * \name Matrices
 * @{
 */

/**
 * Allocates memory for the square matrix \f$A\f$ of dimensions
 * \f$(d+1)\times(d+1)\f$. Initializes its elements to 0.
 */
template <typename Real>
inline void CreateMatr (Real** & A, int d)
{
    A = new Real *[1 + d];
    for (int i = 0; i <= d; i++) {
        A[i] = new Real[1 + d];
        for (int j = 0; j <= d; j++)
            A[i][j] = 0; }
}



/**
 * Frees the memory used by the  \f$(d+1)\times(d+1)\f$ matrix \f$A\f$.
 */
template <typename Real>
inline void DeleteMatr (Real** & A, int d)
{
    for (int i = d; i >= 0; --i)
        delete[] A[i];
    delete[] A;
 //   A = 0;
}



/**
 * Allocates memory for the matrix \f$A\f$ of dimensions
 * (\c line + 1) \f$\times\f$ (\c col + 1). Initializes its elements to 0.
 */
template <typename Real>
inline void CreateMatr (Real** & A, int line, int col)
{
    A = new Real *[1+line];
    for (int i = 0; i <= line; i++) {
        A[i] = new Real[1+col];
        for (int j = 0; j <= col; j++)
            A[i][j] = 0;
    }
}



/**
 * Frees the memory used by the matrix \f$A\f$.
 */
template<typename Real>
inline void DeleteMatr (Real** & A, int line, int col)
{
    for (int i = line; i >= 0; --i)
        delete [] A[i];
    delete [] A;
//    A = 0;
}



/**
 * Creates the square matrix \f$A\f$ of dimensions \f$(d+1)\times(d+1)\f$
 * and initializes its elements to 0.
 */
inline void CreateMatr (MMat& A, int d)
{
    A.SetDims (1 + d, 1 + d);
    //clear (A);
}



/**
 * As above.
 */
inline void CreateMatr (MMatP & A, int d)
{
    A.SetDims (1 + d, 1 + d);   clear (A);
}



/**
 * Creates the matrix \f$A\f$ of dimensions
 * (\c line + 1) \f$\times\f$ (\c col + 1). Initializes its elements to 0.
 */
inline void CreateMatr (MMat& A, int line, int col)
{
    A.SetDims(1 + line, 1 + col);
    //clear (A);
}



/**
 * As above.
 */
inline void CreateMatr (MMatP & A, int line, int col)
{
    A.SetDims (1 + line, 1 + col);   clear (A);
}



/**
 * Deletes the matrix \f$A\f$.
 */
inline void DeleteMatr (MMat& A)
{
    A.kill ();
}



/**
 * As above.
 */
inline void DeleteMatr (MMatP & A)
{
    A.kill ();
}



/**
 * Copies matrix \f$A\f$ into matrix \f$B\f$.
 */
template <typename Matr>
inline void CopyMatr (const Matr & A, Matr & B, int n)
{
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= n; j++)
            B[i][j] = A[i][j];
}



/**
 * As above.
 */
template <typename Matr>
inline void CopyMatr (const Matr & A, Matr & B, int line, int col)
{
    for (int i = 1; i <= line; i++)
        for (int j = 1; j <= col; j++)
            B[i][j] = A[i][j];
}



/**
 * Transforms \c mat into a string. Prints the first \f$d1\f$ rows and
 * \f$d2\f$ columns. Indices start at 1. Elements with index 0 are not printed.
 */
template <typename MatT>
std::string toStr (const MatT & mat, int d1, int d2) 
{
   std::ostringstream ostr;
   for (int i = 1; i <= d1; i++) {
      ostr << "[";
      for (int j = 1; j <= d2; j++) {
         ostr << std::setprecision (2) << std::setw (6) << mat[i][j] <<
            std::setw (2) << " ";
      }
      ostr << "]\n";
   }
   return ostr.str ();
}

/**
 * 
 */
}     // namespace LatMRG
/**
 * @}
 * \name Some other compatibility utilities
 * @{
 */
/**
 * Converts the array of characters (string) \c c into \c l.
 */
namespace NTL {

inline void conv (long & l, const char* c)  {
    l = strtol(c, (char **) NULL, 10);
}


/**
 * Converts the array of characters (string) \c c into \c r.
 */
inline void conv (double & r, const char* c)  {
    r = strtod(c, (char **) NULL);
}

/**
 * @}
 */

}     // namespace

#endif

