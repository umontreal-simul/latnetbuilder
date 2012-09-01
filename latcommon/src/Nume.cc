/*************************************************************************\
 *
 * Package:        LatCommon
 * File:           Nume.cc
 * Environment:    ANSI C
 *
 * Copyright (c) 2011 Pierre L'Ecuyer, DIRO, Université de Montréal.
 * e-mail: lecuyer@iro.umontreal.ca
 *
\*************************************************************************/
#include "latcommon/Nume.h"
#include <cmath>
#include <stdexcept>
// #include <boost/math/constants/constants.hpp>


// PI
#ifndef M_PI
#define M_PI   3.1415926535897932384626433832795029L
#endif

// Euler-Mascheroni gamma
#define M_EULER   0.57721566490153286060651209008240243104215933593992L

using namespace std;

//=========================================================================

namespace {

const double UNSIX = 1.0 / 6.0;
const double QUARAN = 1.0 / 42.0;
const double UNTRENTE = 1.0 / 30.0;
const double DTIERS = 2.0 / 3.0;
const double STIERS = 7.0 / 3.0;
const double QTIERS = 14.0 / 3.0;
const double CTIERS = 5.0 / 3.0;
// const double M_EULER = boost::math::constants::euler<double>();
// const double M_PI = boost::math::constants::pi<double>();


//=========================================================================
#if LONG_MAX > 2147483647L
#define FACT_MAX 20
#else
#define FACT_MAX 12
#endif

/* The factorials n! from n = 0 to n = 12 or n = 20 */
const long larr_Factorials[] = {
   1,
   1,
   2,
   6,
   24,
   120,
   720,
   5040,
   40320,
   362880,
   3628800,
   39916800,
   479001600
#if LONG_MAX > 2147483647L
   ,
   6227020800L,
   87178291200L,
   1307674368000L,
   20922789888000L,
   355687428096000L,
   6402373705728000L,
   121645100408832000L,
   2432902008176640000L
#endif
};


//=========================================================================

#if 0
double sumSnaive (double x, long n)
{
   /* Computes S(x, n) = sum_{h=1}^(n) (cos(2 PI h x))/ h */

   const double z = 2.0 * M_PI * x;
   long h;
   double sum = 0.0;
   if (n < 1)
      return 0.0;

   for (h = 1; h <= n; h++)
      sum += cos (h * z) / h;
   return sum;
}
#endif

double sumS (double x, long n)
{
   /* Computes S(x, n) = sum_{h=1}^(n) (cos(2 PI h x))/ h  */
   /* Uses Clenshaw's algorithm as for Chebyshev series    */

   long h;
   const double z = 2.0 * cos (2.0 * M_PI * x);
   double c0 = 0, c1 = 0, c2;
   if (n < 1)
      return 0.0;

   for (h = n; h > 0; h--) {
      c2 = c1;
      c1 = c0;
      c0 = z * c1 - c2 + 1.0 / h;
   }
   c2 = c1;
   c1 = c0;
   c0 = z * c1 - c2;

   return c0 - 0.5 * c1 * z;
}


double sumH (double x, long n)
{
   /* Computes H(x, n) = sum_{h = n}^infinity (cos(2 PI h x))/ h */
   const double EPS = 1.0e-12;
   const double z = fabs (sin (M_PI * x));
   const long KMAX = 40;
   long k;

   double b = 1.0 / (2 * n * z);
   double sum = 0.0;
   for (k = 1; k <= KMAX; k++) {
      sum += b * cos (M_PI * ((2 * n + k - 2) * x + 0.5 * (k)));
      b *= -(k) / ((n + k) * 2 * z);
      if (fabs (b) <= sum * EPS)
         break;
   }
   return sum;
}

}  // namespace


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//-------------------------------------------------------------------------


namespace LatCommon
{

long lFactorial (int n)
{
   if (n < 0 || n > FACT_MAX )
      throw invalid_argument("lFactorial:   n too large");
   long y = larr_Factorials[n];
   if (y < 0)
      throw overflow_error("lFactorial:   long too small");
   return y;
}


//=========================================================================

double Harmonic (long n)
{
   if (n < 1)
      throw invalid_argument("Harmonic:   n < 1");
   return Digamma ((double) n + 1) + M_EULER;
}


double Harmonic2 (long n)
{
   if (n < 1)
      throw invalid_argument("Harmonic2:   n < 1");
   long k = n / 2;
   if (1 == n)
      return 0.0;
   if (2 == n)
      return 1.0;

   if (n & 1)
      return 2.0 * Harmonic (k); /* n odd */
   return 1.0 / k + 2.0 * Harmonic (k - 1); /* n even */
}


//=========================================================================

double Digamma (double x)
{
   static const double C7[] = {
      1.3524999667726346383e4, 4.5285601699547289655e4,
      4.5135168469736662555e4, 1.8529011818582610168e4,
      3.3291525149406935532e3, 2.4068032474357201831e2,
      5.1577892000139084710, 6.2283506918984745826e-3
   };

   static const double D7[] = {
      6.9389111753763444376e-7, 1.9768574263046736421e4,
      4.1255160835353832333e4, 2.9390287119932681918e4,
      9.0819666074855170271e3, 1.2447477785670856039e3,
      6.7429129516378593773e1, 1.0
   };

   static const double C4[] = {
      -2.728175751315296783e-15, -6.481571237661965099e-1,
      -4.486165439180193579, -7.016772277667586642, -2.129404451310105168
   };

   static const double D4[] = {
      7.777885485229616042, 5.461177381032150702e1,
      8.929207004818613702e1, 3.227034937911433614e1, 1.0
   };

   double prodPj = 0.0;
   double prodQj = 0.0;
   double digX = 0.0;

   if (x >= 3.0) {
      double x2 = 1.0 / (x * x);
      int j;
      for (j = 4; j >= 0; j--) {
         prodPj = prodPj * x2 + C4[j];
         prodQj = prodQj * x2 + D4[j];
      }
      digX = log (x) - (0.5 / x) + (prodPj / prodQj);

   } else if (x >= 0.5) {
      const double X0 = 1.46163214496836234126;
      int j;
      for (j = 7; j >= 0; j--) {
         prodPj = x * prodPj + C7[j];
         prodQj = x * prodQj + D7[j];
      }
      digX = (x - X0) * (prodPj / prodQj);

   } else {
      double f = (1.0 - x) - floor (1.0 - x);
      digX = Digamma (1.0 - x) + M_PI / tan (M_PI * f);
   }

   return digX;
}


//=========================================================================

double FourierC1 (double x, long n)
{
#ifdef HAVE_CBRT
   const double gamma = cbrt (6 * n * (double) n / (M_PI * M_PI));
#else
   const double gamma =
      pow (6 * n * (double) n / (M_PI * M_PI), 1.0 / 3.0);
#endif
   if (x < 0 || x > 1)
      throw invalid_argument("FourierC1:   x not in [0,1]");

   if (n < 1)
      return 0.0;
   if (x > 0.5)
      x = 1.0 - x;
   if (0.0 == x)
      return Harmonic (n);

   if ((x < gamma / n) || (n <= 50))
      return sumS (x, n);
   else
      return -log (2.0 * sin (M_PI * x)) - sumH (x, n + 1);
}


//=========================================================================

double FourierE1 (double x, long n)
{
   if (n <= 1)
      return 0.0;

   if (n & 1)
      return 2.0 * FourierC1 (x, n / 2);
   else {
      return 2.0*FourierC1(x, n/2 - 1) + 2.0*cos(M_PI * n * x)/n;
      /* + i*sin(M_PI * n * x)/k; // assume the imaginary part vanishes */
   }
}


//=========================================================================

double BernoulliPoly (int n, double x)
{
   switch (n) {
   case 0:
      return 1.0;
   case 1:
      return x - 0.5;
   case 2:
      return x * (x - 1.0) + UNSIX;
   case 3:
      return ((2.0*x - 3.0) * x + 1.0)*x*0.5;
   case 4:
      return ((x - 2.0) * x + 1.0) * x * x - UNTRENTE;
   case 5:
      return (((x - 2.5) * x + CTIERS) *x*x - UNSIX) * x;
   case 6:
      return (((x - 3.0) * x + 2.5) * x * x - 0.5) * x * x + QUARAN;
   case 7:
      return ((((x - 3.5) * x + 3.5) *x*x - 7.0/6.0) * x*x + UNSIX) * x;
   case 8:
      return ((((x - 4.0) * x +
               QTIERS) * x * x - STIERS) * x * x + DTIERS) * x * x - UNTRENTE;
   default:
      throw invalid_argument("BernoulliPoly:   n > 8");
      return -1;
   }
   return -1;
}


//=========================================================================
}
