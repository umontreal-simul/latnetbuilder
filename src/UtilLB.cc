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
