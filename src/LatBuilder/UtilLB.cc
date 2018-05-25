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

//===============================================================================
Polynomial PolynomialFromInt(uInteger x)
{
   Polynomial P; 
   uInteger y=x;
   long i=0;
   while(y>0){
      P += Polynomial(INIT_MONO, i, y%2);
      i++;
      y /= 2;
   }
    return P;
}

//================================================================================

uInteger IndexOfPolynomial(Polynomial P)
{
   uInteger x=0;
   uInteger q=1;
   
   for (int i = 0; i <= deg(P); i++)
    { 
       if(IsOne(coeff(P,i))){
            x+= q;
       }
       q *= 2;
    }
    return x;
}



//================================================================================

std::vector<uInteger> primeFactors(uInteger n, bool raise)
{
   std::vector<uInteger> factors;
   uInteger p = 2;
   while (p <= n) {
      ldiv_t qr = std::ldiv(n, p);
      uInteger factor = 1;
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

std::map<uInteger, uInteger> primeFactorsMap(uInteger n)
{
   std::map<uInteger, uInteger> factors;
   uInteger factor = 2;
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

std::pair<long long, long long> egcd(uInteger a, uInteger b)
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


uInteger Vm(const Polynomial& h, const Polynomial& P)
{
   
   long m = deg(P);
   NTL::vector<GF2> w;
   w.resize(m);
   uInteger res = 0;
   for(long i=0; i<m; i++){
      w[i] = (m-i-1>deg(h)) ? GF2(0) : coeff(h,m-i-1);
      for(long j=0; j<i; j++){
         w[i] += w[j]* coeff(P,m-(i-j));
      }  
   }
   uInteger q = 1;
   for(long i=m-1; i>=0; i--){
      res += IsOne(w[i])*q;
      q *= 2;
   }

   return res;

}

uInteger log2Int(unsigned int n){
    unsigned int res=0;
    while (n>1){
    n = n >> 1;
    res ++;
    }
    return res;
}
//================================================================================

} // namespace LatBuilder
