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
 * This module describes useful tools and interface functions to Boost or NTL.
 * 
 * 
 */

#ifndef UTILLC_H
#define UTILLC_H

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <cstdlib>
#include "latcommon/Types.h"
#include "latcommon/Const.h"

#ifdef WITH_NTL
#include "NTL/tools.h"
#include "NTL/ZZ.h"
#endif


namespace LatCommon {


/**
 * Special exit function. \c status is the code to return to the
 * system, \c msg is the message to print on exit.
 */
void MyExit (int status, std::string msg);


/**
 * Table of powers of 2: <tt>TWO_EXP</tt>[\f$i\f$]  \f$= 2^i\f$, \f$i=0, 1, \ldots\f$
 * for \f$i \le 31\f$ for 32-bits machines, and \f$i \le 63\f$ for 64-bits machines.
 */
extern const unsigned long TWO_EXP[];


/**
 * Swaps the values of \c x and \c y.
 */
template <typename T>
inline void swap9 (T & x, T & y)  { T t = x; x = y; y = t; } 


#ifndef WITH_NTL
template <typename A, typename B>
void conv (A & x, const B & y) { x = y; }
/**
 * Converts \f$y\f$ to \f$x\f$.
 */
#endif
/**
 * @}
 * \name NTL compatibility functions
 * @{
 */

inline void clear (double & x) { x = 0; } 
/**
 * Sets \f$x\f$ to 0.
 */
inline void clear (long & x) { x = 0; } 


inline void set9 (long & x) 
{
    x = 1;
}
#ifdef WITH_NTL
inline void set9 (NTL::ZZ & x) 
{
    NTL::set(x);
}

/**
 * Sets \f$x\f$ to 1.
 */
#endif


/**
 * Return \c true if \f$x = 0\f$.
 */
inline bool IsZero (const long & x) 
{
    return x == 0;
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
 */

template <typename Int>
inline void Quotient (const Int & a, const Int & b, Int & q)
{
    q = a/b;
}


#ifdef WITH_NTL
   inline void Quotient (const NTL::ZZ & a, const NTL::ZZ & b, NTL::ZZ & q)
{
    NTL::ZZ r;
    DivRem (q, r, a, b);
    if (q < 0 && r != 0)
       ++q;
}

/**
 * Computes \f$q = a/b\f$ by dropping the fractionnal part, i.e. truncates
 * towards 0. Example:
 * 
 * \begin{center}
 * \begin{tabular}{|r|r|r|}
 * \hline
 * \f$a\f$    & \f$b\f$ & \f$q\f$\\
 * \hline
 * \f$5\f$    & 3 & 1\\
 * \f$-5\f$  &  \f$3\f$  &   \f$-1\f$  \\
 * \f$5\f$  & \f$-3\f$  &  \f$-1\f$  \\
 * \f$-5\f$  & \f$-3\f$  &  \f$1\f$  \\
 * \hline
 * \end{tabular}
 * \end{center}
 */
#endif

#if 0
template <typename Real, typename T>
inline void Modulo (const Real & a, const T & b, Real & r)
{
    std::cout << "Modulo Real non testé" << std::endl;
    exit(1);
    Real br;
    br = b;
    r = fmod (a, br);
    if (r < 0) {
        if (b < 0)
            r -= br;
        else
            r += br;
    }
}
#endif

inline void Modulo (const long & a, const long & b, long & r)
{
    r = a % b;
    if (r < 0) {
        if (b < 0)
            r -= b;
        else
            r += b;
    }
}


#ifdef WITH_NTL
   inline void Modulo (const NTL::ZZ & a, const NTL::ZZ & b, NTL::ZZ & r)
{
    r = a % b;
    if (r < 0)
        r -= b;
}

/**
 * Computes the ``positive'' remainder \f$r = a \bmod b\f$, i.e. such that
 * \f$0 \le r < b\f$. Example:
 * 
 * \begin{center}
 * \begin{tabular}{|r|r|c|}
 * \hline
 * \f$a\f$    & \f$b\f$   & \f$r\f$ \\
 * \hline
 * \f$5\f$   & 3     &  2    \\
 * \f$-5\f$  &  \f$3\f$  &  \f$1\f$  \\
 * \f$5\f$   & \f$-3\f$  &  \f$2\f$  \\
 * \f$-5\f$  & \f$-3\f$  &  \f$1\f$  \\
 * \hline
 * \end{tabular}
 * \end{center}
 */
#endif


/**
 * Computes the quotient \f$q = a/b\f$ and remainder \f$r = a \bmod b\f$. Truncates \f$q\f$ to
 * the nearest integer towards 0. One always has \f$a = qb + r\f$ and \f$|r| < |b|\f$.
 */
template <typename Real>
inline void Divide (Real & q, Real & r, const Real & a, const Real & b)
{
    q = a / b;
    conv (q, trunc(q));
    r = a - q * b;
}


inline void Divide (long & q, long & r, const long & a, const long & b)
{
    ldiv_t z = ldiv (a, b);
    q = z.quot;      // q = a / b;
    r = z.rem;       // r = a % b;
}


#ifdef WITH_NTL
inline void Divide (NTL::ZZ & q, NTL::ZZ & r, const NTL::ZZ & a,
                    const NTL::ZZ & b)
{
    DivRem (q, r, a, b);
    if (q < 0 && r != 0) {
        ++q;
        r -= b;
    }
}

/**
 * Computes the quotient \f$q = a/b\f$ and remainder \f$r = a \bmod b\f$ by
 * truncating \f$q\f$ towards 0. The remainder can be negative.
 * One always has \f$a = qb + r\f$ and
 * \f$|r| < |b|\f$. Example:
 * 
 * \begin{center}
 * \begin{tabular}{|r|r|r|r|}
 * \hline
 * \f$a\f$    & \f$b\f$ & \f$q\f$ & \f$r\f$\\
 * \hline
 * \f$5\f$    & 3 & 1     &   \f$2\f$ \\
 * \f$-5\f$  &  \f$3\f$  &   \f$-1\f$ &   \f$-2\f$  \\
 * \f$5\f$  & \f$-3\f$  &  \f$-1\f$ &   \f$2\f$  \\
 * \f$-5\f$  & \f$-3\f$  &  \f$1\f$ &   \f$-2\f$  \\
 * \hline
 * \end{tabular}
 * \end{center}
 */
#endif


template <typename Real>
inline void DivideRound (const Real & a, const Real & b, Real & q)
{
    q = a/b;
    q = floor(q + 0.5);
}

inline void DivideRound (const long & a, const long & b, long & q)
{
    bool neg;
    if ((a > 0 && b < 0) || (a < 0 && b > 0))
       neg = true;
    else
       neg = false;
    long x = std::abs(a);
    long y = std::abs(b);
    ldiv_t z = ldiv (x, y);
    q = z.quot;
    long r = z.rem;
    r <<= 1;
    if (r > y)
       ++q;
    if (neg)
       q = -q;
}

#ifdef WITH_NTL
inline void DivideRound (const NTL::ZZ & a, const NTL::ZZ & b, NTL::ZZ & q)
{
    bool s = false;
    if ((a > 0 && b < 0) || (a < 0 && b > 0))
       s = true;
    NTL::ZZ r, x, y;
    x = std::abs (a);
    y = std::abs (b);
    //* * ATTENTION: bug de NTL: DivRem change le signe de a quand a < 0.
    DivRem (q, r, x, y);
    LeftShift (r, r, 1);
    if (r > y)
       ++q;
    if (s)
       q = -q;
}

/**
 * Computes \f$a/b\f$, rounds the result to the nearest integer and returns
 * the result in \f$q\f$.
 */
#endif
/**
 * @}
 * \name Mathematical functions
 * @{
 */

/**
 * For given \f$a\f$ and \f$b\f$, returns the values
 * \f$C\f$, \f$D\f$, \f$E\f$, \f$F\f$ and  \f$G\f$ such that:
 * \f{eqnarray*}{
 * C  a + D  b  &=& G = \mbox{GCD } (a,b) \\
 * E  a + F  b  &=& 0.
 * \f}
 */
void Euclide (const MScal & a, const MScal & b, MScal & C, MScal & D,
              MScal & E, MScal & F, MScal & G);

/**
 * Returns the value of the greatest common divisor of \f$a\f$ and \f$b\f$.
 * \todo Richard: Il y a déjà des fonctions GCD dans NTL, pour les \c long
 * et les \c ZZ (voir fichier ZZ.h)
 */
long gcd (long a, long b);


/**
 * Rounds to the nearest integer value.
 */
template <typename Real>
inline Real Round (Real x)
{
    return floor (x + 0.5);
}



/**
 * Returns \c true if \f$n\f$ is prime,  \c false otherwise.
 */
bool IsPrime (unsigned long n);
/**
 * @}
 * \name Vector operations
 * @{
 */

/**
 * Sets components \f$[0..d]\f$ of \f$A\f$ to 0.
 */
template <typename Vect>
inline void SetZero (Vect & A, int d)
{
    for (int i = 0; i <= d; i++)
        A[i] = 0;
}



/**
 * Adds vector \f$B\f$ multiplied by \f$x\f$ to vector \f$A\f$ using components
 * \f$[1..n]\f$, and puts the result in \f$A\f$.
 */
template <typename Vect, typename Scal>
inline void ModifVect (Vect & A, Vect & B, const Scal & x, int n)
{
    MScal y;
    conv (y, x);
    for (int i = 1; i <= n; i++)
       A[i] = A[i] + B[i]*y;
}



/**
 * Adds vector \f$B\f$ multiplied by \f$x\f$ to vector \f$A\f$ using components
 * \f$[1..n]\f$, and puts the result in \f$A\f$.
 */
template <typename Vect, typename Scal>
inline void ModifVect (MVect & A, Vect & B, const Scal & x, int n)
{
    MScal y;
    conv (y, x);
    for (int i = 1; i <= n; i++)
       A[i] = A[i] + B[i]*y;
}



template <typename V, typename Scal>
inline void ProdScal (const V & A, const V & B, int n, Scal & D)
{
    // Le produit A[i] * B[i] peut déborder, d'oÃ¹ conv.
    MScal C;   C = 0;
    for (int i = 1; i <= n; i++)
        C += A[i] * B[i];
    conv (D, C);
}


/**
 * Computes the scalar product of vectors \f$A\f$ and \f$B\f$, using
 * components \f$[1..n]\f$, and puts the result in \f$D\f$.
 */
template <typename Scal>
inline void ProdScal (const BVect & A, const BVect & B, int n, Scal & D)
{
    // Le produit A[i] * B[i] peut déborder, d'oÃ¹ conv.
    MScal C;   C = 0;
    for (int i = 1; i <= n; i++)
        C += A[i] * B[i];
    conv (D, C);
}


/**
 * Changes the sign of the components \f$[1..n]\f$ of vector \f$A\f$.
 */
template <typename Vect>
inline void ChangeSign (Vect & A, int n)
{
    for (int i = 1; i <= n; i++) A[i] = -A[i];
}



/**
 * Computes the greatest common divisor of \f$V[k],\ldots,V[n]\f$.
 */
inline long GCD2vect (std::vector<long> V, int k, int n)
{
    int i = k + 1;
    long r, d, c;
    d = labs (V[k]);
    while (d != 1 && i <= n) {
        c = labs (V[i]);
        while (c) {
            r = d % c;
            d = c;
            c = r;
        }
        ++i;
    }
        return d;
}



/**
 * Computes the \c norm norm of vector \f$V\f$, using components \f$[1..n]\f$,
 * and puts the result in \f$S\f$.
 */
template <typename Vect, typename Scal>
inline void CalcNorm (const Vect & V, int n, Scal & S, NormType norm)
{
    Scal y;
    S = 0;
    switch (norm) {
        case L1NORM:
            for (int i = 1; i <= n; i++) {
                conv (y, V[i]);
                S += std::abs(y);
            }
            break;

        case L2NORM:
            for (int i = 1; i <= n; i++) {
                conv (y, V[i]);
                S += y*y;
            }
            //S = sqrt(S);
            break;

        case SUPNORM:
            for (int i = 1; i <= n; i++) {
                conv (y, std::abs(V[i]));
                if (y > S)
                    S = y;
            }
            break;

        case ZAREMBANORM:
            S = 1.0;
            for (int i = 1; i <= n; i++) {
                conv (y, std::abs(V[i]));
                if (y > 1.0)
                   S *= y;
            }
            break;

        default:
            ;
    }
}

/**
 * @}
 * \name Matrices
 * @{
 */

/**
 * Checks that square matrix \f$A\f$ is upper triangular (modulo \f$m\f$) for
 * dimensions 1 to \c dim.
 */
template <typename Matr>
bool CheckTriangular (const Matr & A, int dim, const MScal & m) 
{
   for (int i = 2; i <= dim; i++) {
      for (int j = 1; j < i; j++) {
         if (A(i,j) % m != 0) {
            std::cout << "******  CheckTriangular failed for element A[" << i <<
                 "][" << j << "]" << std::endl;
            return false;
         }
      }
    }
    return true;
}


/**
 * Performs an integer triangularization operation modulo \f$m\f$ on the matrix \f$W\f$
 * to obtain an upper triangular matrix \f$V\f$, dual to \f$W\f$. However, the matrix \f$W\f$
 * will be transformed too in order to preserve duality. Only the first
 * \c lin lines and the first \c col columns of the matrices will
 * be considered (counting indices from 1; line 0 and column 0 must be there
 * but are unused).
 */
template <typename Matr>
void Triangularization (Matr & W, Matr & V, int lin, int col,
                        const MScal & m) 
{
   MScal T1, T2, T3, T4, T5, T6, T7, T8;

   for (int j = 1; j <= col; j++) {
      for (int i = 1; i <= lin; i++)
         Modulo (W(i,j), m, W(i,j));
      int r = 1;
      while (r < lin) {
         while (IsZero (W(r,j)) && r < lin)
            ++r;
         if (r < lin) {
            int s = r + 1;
            while (IsZero (W(s,j)) && s < lin)
               ++s;
            if (!IsZero (W(s,j))) {
               Euclide (W(r,j), W(s,j), T1, T2, T3, T4, W(s,j));
               clear (W(r,j));
               for (int j1 = j + 1; j1 <= col; j1++) {
                  T5 = T1 * W(r,j1);
                  T6 = T2 * W(s,j1);
                  T7 = T3 * W(r,j1);
                  T8 = T4 * W(s,j1);
                  W(s,j1) = T5 + T6;
                  Modulo (W(s,j1), m, W(s,j1));
                  W(r,j1) = T7 + T8;
                  Modulo (W(r,j1), m, W(r,j1));
               }
            } else {
               for (int j1 = j; j1 <= col; j1++) {
                  std::swap (W(r,j1), W(s,j1));
               }
            }
            r = s;
         }
      }
      if (IsZero (W(lin,j))) {
         for (int j1 = 1; j1 <= col; j1++) {
            if (j1 != j)
               clear (V(j,j1));
            else
               V(j,j1) = m;
         }
      } else {
         Euclide (W(lin,j), m, T1, T2, T3, T4, V(j,j));
         for (int j1 = 1; j1 < j; j1++)
            clear (V(j,j1));
         for (int j1 = j + 1; j1 <= col; j1++) {
            T2 = W(lin,j1) * T1;
            Modulo (T2, m, V(j,j1));
         }
         Quotient (m, V(j,j), T1);
         for (int j1 = j + 1; j1 <= col; j1++) {
            W(lin,j1) *= T1;
            Modulo (W(lin,j1), m, W(lin,j1));
         }
      }
   }
//   CheckTriangular (V, col, m);
}


/**
 * Calculates the \f$m\f$-dual of the matrix \c A. The result is placed in the
 * matrix \c B. Only the first \f$d\f$ lines and columns are considered.
 */
template <typename Matr>
void CalcDual (const Matr & A, Matr & B, int d, const MScal & m) 
{
   for (int i = 1; i <= d; i++) {
      for (int j = i + 1; j <= d; j++)
         clear (B(i,j));
// Dans l'original, c'est Quotient pour Lac et DivideRound pour non-Lac ??
//        Quotient(m, A(i,i), B(i,i));
      DivideRound (m, A(i,i), B(i,i));
      for (int j = i - 1; j >= 1; j--) {
         clear (B(i,j));
         for (int k = j + 1; k <= i; k++) {
            B(i,j) += A(j,k) * B(i,k);
         }
         if (B(i,j) != 0)
            B(i,j) = -B(i,j);
// Dans l'original, c'est Quotient pour Lac et DivideRound pour non-Lac ??
//         Quotient(B(i,j), A(j,j), B(i,j));
         DivideRound (B(i,j), A(j,j), B(i,j));
      }
   }
}
/**
 * @}
 * \name Printing functions and operators
 * @{
 */

/**
 * Prints components \f$[c..d]\f$ of vector \f$A\f$ as a string. Components are
 * separated by string \c sep.
 */
template <typename Vect>
std::string toString (const Vect & A, int c, int d, const char* sep)
{
    std::ostringstream out;
    out << "[";
    for (int i = c; i < d; i++)
        out << A[i] << sep;
    out << A[d] << "]";
    return out.str ();
}



/**
 * Prints components \f$[c..d]\f$ of vector \f$A\f$ as a string.
 */
template <typename Vect>
inline std::string toString (const Vect & A, int c, int d)
{
    return toString (A, c, d, "  ");
}



/**
 * Prints components \f$[1..d]\f$ of vector \f$A\f$ as a string.
 */
template <typename Vect>
std::string toString (const Vect & A, int d)
{
   std::ostringstream ostr;
   ostr << "[";
   for (int i = 1; i <= d; i++) {
      ostr << std::setprecision(2) << std::setw(3) << A[i] <<
              std::setw(2) << "  ";
   }
   ostr << "]";
   return ostr.str();
}



/**
 * Streaming operator for vectors.
 * Formats a pair as: \c (first,second).
 */
template <class T1, class T2>
std::ostream & operator<< (std::ostream & out, const std::pair<T1,T2> & x) 
{
   out << "(" << x.first << "," << x.second << ")";
   return out;
}


/**
 * Streaming operator for vectors.
 * Formats a vector as: <tt>[ val1, ..., valN ]</tt>.
 */
template <class T, class A>
std::ostream & operator<< (std::ostream & out, const std::vector<T,A> & x) 
{
   out << "[";
   typename std::vector<T,A>::const_iterator it = x.begin();
   if (it != x.end()) {
      out << *it;
      while (++it != x.end())
         out << ", " << *it;
   }
   out << "]";
   return out;
}


/**
 * Streaming operator for sets.
 * Formats a set as: \texttt{\{ val1, ..., valN \}}.
 */
template <class K, class C, class A>
std::ostream & operator<< (std::ostream & out, const std::set<K,C,A> & x) 
{
   out << "{";
   typename std::set<K,C,A>::const_iterator it = x.begin();
   if (it != x.end()) {
      out << *it;
      while (++it != x.end())
         out << ", " << *it;
   }
   out << "}";
   return out;
}


/**
 * Streaming operator for maps.
 * Formats a map as: \texttt{\{ key1=>val1, ..., keyN=>valN \}}.
 */
template <class K, class T, class C, class A>
std::ostream & operator<< (std::ostream & out, const std::map<K,T,C,A> & x) 
{
   out << "{";
   typename std::map<K,T,C,A>::const_iterator it = x.begin();
   if (it != x.end()) {
      out << it->first << "=>" << it->second;
      while (++it != x.end())
         out << ", " << it->first << "=>" << it->second;
   }
   out << "}";
   return out;
}
}

#endif

