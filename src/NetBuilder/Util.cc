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

#include "netbuilder/Util.h"
#include "latbuilder/Parser/SizeParam.h"
#include "latbuilder/Parser/Common.h"
#include <assert.h>

namespace NetBuilder {

struct OpAdd{
    Real operator()(Real x, Real y){return x+y;}
};

struct OpMax{
    Real operator()(Real x, Real y){ return (x < y) ? y : x;}
};

BinOp realToBinOp(Real normType){
if (normType < std::numeric_limits<Real>::infinity())
{
    return BinOp(OpAdd());
}
return BinOp(OpMax());
}


std::vector<std::vector<int>> compositions(int n, int nb_parts){
    assert(nb_parts>= 2 && n>= nb_parts);
    if (nb_parts == 2){
        std::vector<std::vector<int>> v;
        for (int k=n-1; k>0; k--){
            v.push_back(std::vector<int>({k, n-k}));
        }
        return v;
    }
    else{
        std::vector<std::vector<int>> V_grand;
        for (int k=n-nb_parts+1; k>=1; k--){
            auto V_petit = compositions(n-k, nb_parts-1);
            for (std::vector<int> v: V_petit){
                if (k & 1){   // k is odd
                    v.insert(v.begin(), k);
                }
                else{
                    std::reverse(v.begin(),v.end());
                    v.insert(v.begin(), k);
                }
                V_grand.push_back(v);
            }
        }
        return V_grand;
    }
}

Row permutation(Row& row, std::vector<int>& C){
    Row new_row(row.size());
    for (unsigned int i=0; i < row.size(); i++){
        new_row[i] = row[C[i]];
    }
    return new_row;
}

Polynomial polynomialParserHelper(const std::string& str)
{
   auto n = LatBuilder::Parser::splitPair<std::string, unsigned int>(str, '^', 0);
   std::string str_NTLInput = LatBuilder::Parser::LatticeParametersParseHelper<LatBuilder::LatticeType::POLYNOMIAL>::ToParsableModulus(n.first);
   try {
      Polynomial base = boost::lexical_cast<Polynomial>(str_NTLInput);      
      if (n.second == 0)
         return base;
      else
         return intPow(base, n.second);
   }
   catch (boost::bad_lexical_cast&) {
      throw LatBuilder::Parser::ParserError("cannot interpret \"" + n.first + "\" as a polynomial");
   }
}



}

