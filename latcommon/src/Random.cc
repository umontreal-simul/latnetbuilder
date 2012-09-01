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

#include "latcommon/Random.h"
#include <climits>


#define GERME   (ULONG_MAX / 54321)
#define NORM53  1.11022302462515654e-16  /* 1/2^53 */
#define NORM32  2.3283064365386963e-10   /* 1/2^32 */
#define MASK53  0x1fffffffffffffUL       /* 2^53 - 1 */

#if ULONG_MAX > 4294967295UL
#define ULONG_64_OUI
#endif

namespace LatCommon
{

Random::Random ()
{
   etat1 = GERME;
   etat2 = GERME;
   etat3 = GERME;
   etat4 = GERME;
   etat5 = GERME;
}


//========================================================================

unsigned long Random::randValue ()
{
   unsigned long b;

#ifdef ULONG_64_OUI
   // Générateur LFSR258 de L'Ecuyer
   b = ((etat1 << 1) ^ etat1) >> 53;
   etat1 = ((etat1 & 18446744073709551614UL) << 10) ^ b;
   b = ((etat2 << 24) ^ etat2) >> 50;
   etat2 = ((etat2 & 18446744073709551104UL) << 5) ^ b;
   b = ((etat3 << 3) ^ etat3) >> 23;
   etat3 = ((etat3 & 18446744073709547520UL) << 29) ^ b;
   b = ((etat4 << 5) ^ etat4) >> 24;
   etat4 = ((etat4 & 18446744073709420544UL) << 23) ^ b;
   b = ((etat5 << 3) ^ etat5) >> 33;
   etat5 = ((etat5 & 18446744073701163008UL) << 8) ^ b;
   return (etat1 ^ etat2 ^ etat3 ^ etat4 ^ etat5);

#else
   // Générateur LFSR113 de L'Ecuyer
   b  = ((etat1 << 6) ^ etat1) >> 13;
   etat1 = ((etat1 & 4294967294UL) << 18) ^ b;
   b  = ((etat2 << 2) ^ etat2) >> 27;
   etat2 = ((etat2 & 4294967288UL) << 2) ^ b;
   b  = ((etat3 << 13) ^ etat3) >> 21;
   etat3 = ((etat3 & 4294967280UL) << 7) ^ b;
   b  = ((etat4 << 3) ^ etat4) >> 12;
   etat4 = ((etat4 & 4294967168UL) << 13) ^ b;
   return (etat1 ^ etat2 ^ etat3 ^ etat4);

#endif
}


//=========================================================================

int Random::randInt (int i, int j)
{
#ifdef ULONG_64_OUI
   unsigned long d = j - i + 1;
   unsigned long q = 0x4000000000000000L / d;
   unsigned long r = 0x4000000000000000L % d;
   unsigned long v = 0x4000000000000000L - r;
   unsigned long res;

   do {
      res = randValue() >> 2;
   } while (res >= v);

   return i + (int) (res / q);

#else
   // moins précise que ci-dessus
   double res = randU01();
   int d = j - i + 1;
   return i + static_cast<long>(d * res);
#endif
}


//=========================================================================

double Random::randU01 ()
{
#ifdef ULONG_64_OUI
   //   return NORM53 * (randValue () >> 11);
   return NORM53 * (randValue () & MASK53);
#else
   return NORM32 * randValue ();
#endif
}


//=========================================================================

unsigned long Random::randBits (int s)
{
#ifdef ULONG_64_OUI
   return randValue () >> (64 - s);
#else
   return randValue () >> (32 - s);
#endif
}


//=========================================================================

void Random::setSeed (unsigned long seed)
{
   // Set one high bit = 1 to make sure initial state is valid
   etat1 = seed | 0x40000000;
   etat2 = seed | 0x40000000;
   etat3 = GERME;
   etat4 = GERME;
   etat5 = GERME;
}


//========================================================================

}
