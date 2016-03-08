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

#include "latbuilder/Util.h"
#include <cmath>
#include <cstdlib>

namespace LatBuilder
{

//================================================================================

Modulus modularPow(Modulus base, Modulus exponent, Modulus modulus)
{
   Modulus result = 1;
   while (exponent) {
      if (exponent % 2 == 1)
         result = (result * base) % modulus;
      exponent /= 2;
      base = (base * base) % modulus;
   }
   return result;
}

//================================================================================

std::vector<Modulus> primeFactors(Modulus n, bool raise)
{
   std::vector<Modulus> factors;
   Modulus p = 2;
   while (p <= n) {
      ldiv_t qr = std::ldiv(n, p);
      Modulus factor = 1;
      while (qr.rem == 0) {
         n = qr.quot;
         factor *= p;
         if (!raise)
            break;
         qr = std::ldiv(n, p);
      }
      if (factor > 1)
         factors.push_back(factor);
      if (p == 2)
         p = 3;
      else
         p += 2;
   }
   return factors;
}

//================================================================================

std::map<Modulus, Modulus> primeFactorsMap(Modulus n)
{
   std::map<Modulus, Modulus> factors;
   Modulus factor = 2;
   while (factor <= n) {
      ldiv_t qr = std::ldiv(n, factor);
      while (qr.rem == 0) {
         n = qr.quot;
         factors[factor]++;
         qr = std::ldiv(n, factor);
      }
      if (factor == 2)
         factor = 3;
      else
         factor += 2;
   }
   return factors;
}

//================================================================================

std::pair<long long, long long> egcd(Modulus a, Modulus b)
{
   typedef std::pair<long long, long long> result_type;
   result_type cur{0, 1};
   result_type last{1, 0};
   while (b != 0) {
      ldiv_t qr = std::ldiv(a, b);
      a = b;
      b = qr.rem;
      std::swap(cur, last);
      cur.first = cur.first - qr.quot * last.first;
      cur.second = cur.second - qr.quot * last.second;
   }
   return last;
}

//================================================================================

} // namespace LatBuilder
