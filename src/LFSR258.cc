// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "latbuilder/LFSR258.h"
#include <limits>

namespace LatBuilder {

const LFSR258::seed_type LFSR258::default_seed = {{
   std::numeric_limits<result_type>::max() / 54321,
   std::numeric_limits<result_type>::max() / 54321,
   std::numeric_limits<result_type>::max() / 54321,
   std::numeric_limits<result_type>::max() / 54321,
   std::numeric_limits<result_type>::max() / 54321
}};

auto LFSR258::operator()() -> result_type
{
   result_type b;
   b = ((m_s[0] << 1) ^ m_s[0]) >> 53;
   m_s[0] = ((m_s[0] & 18446744073709551614UL) << 10) ^ b;
   b = ((m_s[1] << 24) ^ m_s[1]) >> 50;
   m_s[1] = ((m_s[1] & 18446744073709551104UL) << 5) ^ b;
   b = ((m_s[2] << 3) ^ m_s[2]) >> 23;
   m_s[2] = ((m_s[2] & 18446744073709547520UL) << 29) ^ b;
   b = ((m_s[3] << 5) ^ m_s[3]) >> 24;
   m_s[3] = ((m_s[3] & 18446744073709420544UL) << 23) ^ b;
   b = ((m_s[4] << 3) ^ m_s[4]) >> 33;
   m_s[4] = ((m_s[4] & 18446744073701163008UL) << 8) ^ b;
   return (m_s[0] ^ m_s[1] ^ m_s[2] ^ m_s[3] ^ m_s[4]);
}

void LFSR258::jump()
{
   // Les operations qui suivent permettent de faire sauter en avant
   // de 2^100 iterations chacunes des composantes du generateur.
   // L'etat interne apres le saut est cependant legerement different
   // de celui apres 2^100 iterations puisqu'il ignore l'etat dans
   // lequel se retrouvent les premiers bits de chaque composantes,
   // puisqu'ils sont ignores dans la recurrence. L'etat redevient
   // identique a ce que l'on aurait avec des iterations normales
   // apres un appel a nextValue().

   result_type z, b;

   z = m_s[0] & 0xfffffffffffffffeL;
   b = z ^ (z << 1);
   z = (b >> 58) ^ (b >> 55) ^ (b >> 46) ^ (b >> 43) ^ (z << 5) ^
      (z << 8) ^ (z << 17) ^ (z << 20);
   m_s[0] = z;


   z = m_s[1] & 0xfffffffffffffe00L;
   b = z ^ (z << 24);
   z = (b >> 54) ^ (b >> 53) ^ (b >> 52) ^ (b >> 50) ^ (b >> 49) ^
      (b >> 48) ^ (b >> 43) ^ (b >> 41) ^ (b >> 38) ^ (b >> 37) ^
      (b >> 30) ^ (b >> 25) ^ (b >> 24) ^ (b >> 23) ^ (b >> 19) ^
      (b >> 16) ^ (b >> 15) ^ (b >> 14) ^ (b >> 13) ^ (b >> 11) ^
      (b >> 8) ^ (b >> 7) ^ (b >> 5) ^ (b >> 3) ^ (z << 0) ^
      (z << 2) ^ (z << 3) ^ (z << 6) ^ (z << 7) ^ (z << 8) ^ (z << 9) ^
      (z << 10) ^ (z << 11) ^ (z << 12) ^ (z << 13) ^ (z << 14) ^
      (z << 16) ^ (z << 18) ^ (z << 19) ^ (z << 21) ^ (z << 25) ^
      (z << 30) ^ (z << 31) ^ (z << 32) ^ (z << 36) ^ (z << 39) ^
      (z << 40) ^ (z << 41) ^ (z << 42) ^ (z << 44) ^ (z << 47) ^
      (z << 48) ^ (z << 50) ^ (z << 52);
   m_s[1] = z;


   z = m_s[2] & 0xfffffffffffff000L;
   b = z ^ (z << 3);
   z = (b >> 50) ^ (b >> 49) ^ (b >> 46) ^ (b >> 42) ^ (b >> 40) ^
      (b >> 39) ^ (b >> 38) ^ (b >> 37) ^ (b >> 36) ^ (b >> 32) ^
      (b >> 29) ^ (b >> 28) ^ (b >> 27) ^ (b >> 25) ^ (b >> 23) ^
      (b >> 20) ^ (b >> 19) ^ (b >> 15) ^ (b >> 12) ^ (b >> 11) ^
      (b >> 2) ^ (z << 1) ^ (z << 2) ^ (z << 3) ^ (z << 6) ^ (z << 10) ^
      (z << 12) ^ (z << 13) ^ (z << 14) ^ (z << 15) ^ (z << 16) ^
      (z << 20) ^ (z << 23) ^ (z << 24) ^ (z << 25) ^ (z << 27) ^
      (z << 29) ^ (z << 32) ^ (z << 33) ^ (z << 37) ^ (z << 40) ^
      (z << 41) ^ (z << 50);
   m_s[2] = z;


   z = m_s[3] & 0xfffffffffffe0000L;
   b = z ^ (z << 5);
   z = (b >> 46) ^ (b >> 44) ^ (b >> 42) ^ (b >> 41) ^ (b >> 40) ^
      (b >> 38) ^ (b >> 36) ^ (b >> 32) ^ (b >> 30) ^ (b >> 25) ^
      (b >> 18) ^ (b >> 16) ^ (b >> 15) ^ (b >> 14) ^ (b >> 12) ^
      (b >> 11) ^ (b >> 10) ^ (b >> 9) ^ (b >> 8) ^ (b >> 6) ^
      (b >> 5) ^ (b >> 4) ^ (b >> 3) ^ (b >> 2) ^ (z << 2) ^
      (z << 5) ^ (z << 6) ^ (z << 7) ^ (z << 9) ^ (z << 11) ^ (z << 15) ^
      (z << 17) ^ (z << 22) ^ (z << 29) ^ (z << 31) ^ (z << 32) ^
      (z << 33) ^ (z << 35) ^ (z << 36) ^ (z << 37) ^ (z << 38) ^
      (z << 39) ^ (z << 41) ^ (z << 42) ^ (z << 43) ^ (z << 44) ^
      (z << 45);
   m_s[3] = z;


   z = m_s[4] & 0xffffffffff800000L;
   b = z ^ (z << 3);
   z = (b >> 40) ^ (b >> 29) ^ (b >> 10) ^ (z << 1) ^ (z << 12) ^
      (z << 31);
   m_s[4] = z;
}

void LFSR258::check_seed(const seed_type& s)
{
   if ((s[0] < 2)  or
         (s[1] < 512)  or
         (s[2] < 4096) or
         (s[3] < 131072) or
         (s[4] < 8388608))
      throw std::invalid_argument("The seed elements must be either negative "
            "or greater than 1, 511, 4095, 131071 and 8388607 respectively");
}

}
