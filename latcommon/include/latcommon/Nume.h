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
 * This module implements a few useful mathematical functions.
 * 
 * 
 */

#ifndef NUME_H
#define NUME_H


namespace LatCommon {

/**
 * Calculates \f$t!\f$, the factorial of \f$t\f$.
 */
long lFactorial (int t);

/**
 * Returns the value of the logarithmic derivative of the Gamma function
 * \f$\psi(x) = \Gamma'(x) / \Gamma(x)\f$.
 */
double Digamma (double x);

/**
 * Evaluates the Bernoulli polynomial \f$B_n(x)\f$ of degree \f$n\f$
 * at \f$x\f$.
 * The first Bernoulli polynomials are:
 * \f{align*}{
 * B_0(x) &= 1  \\
 * B_1(x) &= x - 1/2  \\
 * B_2(x) &= x^2-x+1/6  \\
 * B_3(x) &= x^3 - 3x^2/2 + x/2  \\
 * B_4(x) &= x^4-2x^3+x^2-1/30 \\
 * B_5(x) &= x^5 - 5x^4/2 + 5x^3/3 - x/6  \\
 * B_6(x) &= x^6-3x^5+5x^4/2-x^2/2+1/42  \\
 * B_7(x) &= x^7 - 7x^6/2 +  7x^5/2 - 7x^3/6 + x/6  \\
 * B_8(x) &= x^8-4x^7+14x^6/3 - 7x^4/3 +2x^2/3-1/30. 
 * \f}
 * Only degrees \f$n \le 8\f$ are programmed for now.
 */
double BernoulliPoly (int n, double x);

/**
 * Computes the \f$n\f$-th harmonic number \f$H_n  = \sum_{j=1}^n 1/j\f$.
 */
double Harmonic (long n);

/**
 * Computes the sum
 * \f[
 * \sideset{}{'}\sum_{-n/2<j\le n/2}\; \frac 1{|j|},
 * \f]
 * where the symbol \f$\sum^\prime\f$ means that the term with \f$j=0\f$ is excluded
 * from the sum.
 */
double Harmonic2 (long n);

/**
 * Computes and returns the value of the series (see \cite vJOE92b)
 * \f[
 * S(x, n) = \sum_{j=1}^{n} \frac{\cos(2\pi j x)}{j}.
 * \f]
 * Restrictions: \f$n>0\f$ and \f$0 \le x \le 1\f$.
 */
double FourierC1 (double x, long n);

/**
 * Computes and returns the value of the series
 * \f[
 * G(x, n) = \sideset{}{'}\sum_{-n/2<h\le n/2}\;  \frac{e^{2\pi i h x}}{|h|},
 * \f]
 * where the symbol \f$\sum^\prime\f$ means that the term with \f$h=0\f$ is excluded
 * from the sum, and assuming that the imaginary part of \f$G(x, n)\f$ vanishes.
 * Restrictions: \f$n>0\f$ and \f$0 \le x \le 1\f$.
 */
double FourierE1 (double x, long n);
}

#endif

