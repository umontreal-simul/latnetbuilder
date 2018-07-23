// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
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
#include "netbuilder/Const.h"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <boost/filesystem.hpp>

#include <NTL/ZZ.h>

namespace LatBuilder
{

//===============================================================================
Polynomial PolynomialFromInt(uInteger x)
{
   Polynomial P;
   P.SetLength(LENGTH_UINTEGER); 
   uInteger y=x;
   long i=0;
    do{
       P[i++] = y%2;
   } while(y >>= 1);
    
    P.normalize();
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
       q <<= 1;
    }
    return x;
}



//================================================================================

std::vector<uInteger> primeFactors(uInteger n, bool raise)
{
   PrimeSeq s;
   unsigned long p;
   p = s.next();
   std::vector<uInteger> factors;
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
      p = s.next();
   }
   return factors;
}

//================================================================================

std::map<uInteger, uInteger> primeFactorsMap(uInteger n)
{
   PrimeSeq s;
   unsigned long factor;
   factor = s.next();
   std::map<uInteger, uInteger> factors;
   while (factor <= n) {
      ldiv_t qr = std::ldiv(n, factor);
      while (qr.rem == 0) {
         n = qr.quot;
         factors[factor]++;
         qr = std::ldiv(n, factor);
      }
      factor = s.next();
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

const char* ws = " \t\n\r\f\v";

// trim from end (right)
inline std::string& rtrim(std::string& s, const char* t = ws)
{
      s.erase(s.find_last_not_of(t) + 1);
      return s;
}

// trim from beginning (left)
inline std::string& ltrim(std::string& s, const char* t = ws)
{
      s.erase(0, s.find_first_not_of(t));
      return s;
}

// trim from both ends (left & right)
inline std::string& trim(std::string& s, const char* t = ws)
{
      return ltrim(rtrim(s, t), t);
}    

std::string getDefaultPolynomial(unsigned int degree)
{
    if (degree <= 32)
    {
        std::string path = NetBuilder::PATH_TO_LATNETBUILDER + "/../share/latnetbuilder/data/default_polys.csv";
        if (boost::filesystem::exists(path)){
            std::ifstream file(path);
            std::string sent;
            do
            {
            getline(file,sent);
            trim(sent);
            }
            while (sent != "###");
        
            getline(file,sent);

            for(unsigned int d = 0; d <= degree; ++d)
            {
                getline(file,sent);
            }
            return sent;
        }
        else{
            throw runtime_error("Unable to locate data folder. The value of PATH_TO_LATNETBUILDER is probably incorrect. See netbuilder/Const.h.");
        }
    }
    return "";
}

} // namespace LatBuilder
