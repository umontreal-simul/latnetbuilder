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

#include "latcommon/UtilLC.h"
#include <cstdlib>
#include <cmath>
#include <climits>


namespace
{
}                                 // namespace

//============================================================================


namespace LatCommon
{

const unsigned long TWO_EXP[] = {
1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768,
65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216,
33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648UL
#if ULONG_MAX > 4294967295UL
,
4294967296, 8589934592, 17179869184, 34359738368, 68719476736, 137438953472,
274877906944, 549755813888, 1099511627776, 2199023255552, 4398046511104,
8796093022208, 17592186044416, 35184372088832, 70368744177664,
140737488355328, 281474976710656, 562949953421312, 1125899906842624,
2251799813685248, 4503599627370496, 9007199254740992, 18014398509481984,
36028797018963968, 72057594037927936, 144115188075855872, 288230376151711744,
576460752303423488, 1152921504606846976, 2305843009213693952,
4611686018427387904UL, 9223372036854775808UL
#endif
};


//============================================================================
// binary GCD from wikipedia

long gcd (long u, long v)
{
   int shift;
   if (u < 0)
      u = -u;
   if (v < 0)
      v = -v;

   // GCD(0,x) := x
   if (u == 0 || v == 0)
      return u | v;

   /* Let shift := lg K, where K is the greatest power of 2 dividing both u
      and v. */
   for (shift = 0; ((u | v) & 1) == 0; ++shift) {
      u >>= 1;
      v >>= 1;
   }

   while ((u & 1) == 0)
      u >>= 1;

   // From here on, u is always odd.
   do {
      while ((v & 1) == 0)     // Loop X
         v >>= 1;

      /* Now u and v are both odd, so diff(u, v) is even. Let u = min(u,
         v), v = diff(u, v)/2. */
      if (u < v) {
         v -= u;
      } else {
         long diff = u - v;
         u = v;
         v = diff;
      }
      v >>= 1;
   } while (v != 0);

   return u << shift;
}


//===========================================================================

void Euclide (const MScal & A, const MScal & B, MScal & C, MScal & D, MScal & E,
              MScal & F, MScal & G)
{
   MScal X, Y, Z;
   G = A;
   Z = B;
   set9 (C);
   clear (D);
   clear (E);
   set9 (F);

   if (IsZero(A)) {
      swap9<MScal>(G, Z);
      swap9<MScal>(C, E);
      swap9<MScal>(D, F);
      return;
   }

   while (!IsZero(Z)) {
      swap9<MScal>(G, Z);
      swap9<MScal>(C, E);
      swap9<MScal>(D, F);
      Quotient (Z, G, X);
      X = -X;
      Y = X * G;
      Z += Y;
      Y = X * C;
      E += Y;
      Y = X * D;
      F += Y;
   }
}


//===========================================================================

void MyExit (int status, std::string msg)
{
   std::cout << "\n***** Error " << msg << std::endl;
   exit (status);
}


//===========================================================================

bool IsPrime (unsigned long n)
{
   if (n == 2)
      return true;
   if ((n & 1) == 0)
      return false;
   unsigned long FACTMAX = static_cast < unsigned long >(std::sqrt (n));
   for (unsigned long i = 3; i <= FACTMAX; i += 2)
      if (n % i == 0)
         return false;
   return true;
}


//===========================================================================

}
