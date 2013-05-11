// Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
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

#include "latbuilder/LFSR113.h"
#include <limits>

namespace LatBuilder {

const LFSR113::seed_type LFSR113::default_seed = {{
   std::numeric_limits<result_type>::max() / 54321,
   std::numeric_limits<result_type>::max() / 54321,
   std::numeric_limits<result_type>::max() / 54321,
   std::numeric_limits<result_type>::max() / 54321
}};

auto LFSR113::operator()() -> result_type
{
   result_type b;
   b  = (((m_s[0] <<   6) ^ m_s[0]) >> 13);
   m_s[0] = (((m_s[0] &   -2) << 18) ^ b);
   b  = (((m_s[1] <<   2) ^ m_s[1]) >> 27);
   m_s[1] = (((m_s[1] &   -8) <<  2) ^ b);
   b  = (((m_s[2] <<  13) ^ m_s[2]) >> 21);
   m_s[2] = (((m_s[2] &  -16) <<  7) ^ b);
   b  = (((m_s[3] <<   3) ^ m_s[3]) >> 12);
   m_s[3] = (((m_s[3] & -128) << 13) ^ b);
   return m_s[0] ^ m_s[1] ^ m_s[2] ^ m_s[3];
}

void LFSR113::jump()
{
   // advance the generator approximately by 2^55 iterations

   int z, b;

   z = m_s[0] & -2;
   b = (z <<  6) ^ z;
   z = (z) ^ (z << 3) ^ (z << 4) ^ (z << 6) ^ (z << 7) ^
      (z << 8) ^ (z << 10) ^ (z << 11) ^ (z << 13) ^ (z << 14) ^
      (z << 16) ^ (z << 17) ^ (z << 18) ^ (z << 22) ^
      (z << 24) ^ (z << 25) ^ (z << 26) ^ (z << 28) ^ (z << 30);
   z ^= (b >> 1) ^ (b >> 3) ^ (b >> 5) ^ (b >> 6) ^
      (b >> 7) ^ (b >> 9) ^ (b >> 13) ^ (b >> 14) ^
      (b >> 15) ^ (b >> 17) ^ (b >> 18) ^ (b >> 20) ^
      (b >> 21) ^ (b >> 23) ^ (b >> 24) ^ (b >> 25) ^
      (b >> 26) ^ (b >> 27) ^ (b >> 30);
   m_s[0] = z;


   z = m_s[1] & -8;
   b = z ^ (z << 1);
   b ^= (b << 2);
   b ^= (b << 4);
   b ^= (b << 8);

   b <<= 8;
   b ^= (z << 22) ^ (z << 25) ^ (z << 27);
   if ((z & 0x80000000) != 0) b ^= 0xABFFF000;
   if ((z & 0x40000000) != 0) b ^= 0x55FFF800;
   z = b ^ (z >> 7) ^ (z >> 20) ^ (z >> 21);
   m_s[1] = z;


   z = m_s[2] & -16;
   b = (z <<  13) ^ z;
   z = (b >> 3) ^ (b >> 17) ^
      (z << 10) ^ (z << 11) ^ (z << 25);
   m_s[2] = z;


   z = m_s[3] & -128;
   b = (z <<  3) ^ z;
   z = (z << 14) ^ (z << 16) ^ (z << 20) ^
      (b >> 5) ^ (b >> 9) ^ (b >> 11);
   m_s[3] = z;
}

void LFSR113::check_seed(const seed_type& s)
{
   if ((s[0] >= 0 && s[0] < 2)  or
         (s[1] >= 0 && s[1] < 8)  or
         (s[2] >= 0 && s[2] < 16) or
         (s[3] >= 0 && s[3] < 128))
      throw std::invalid_argument("The seed elements must be either negative "
            "or greater than 1, 7, 15 and 127, respectively");
}

}
