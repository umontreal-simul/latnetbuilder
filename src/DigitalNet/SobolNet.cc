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

#include "latbuilder/DigitalNet/SobolNet.h"
#include <boost/dynamic_bitset.hpp>

using namespace LatBuilder::DigitalNet;

typedef LatBuilder::uInteger uInteger; 

PrimitivePolynomial SobolNet::nthPrimitivePolynomial(uInteger n){

    constexpr unsigned int degrees[] =
    {
        #include "data/primitive_polynomials_degrees.csv"
    };

    constexpr unsigned int representations[] =
    {
        #include "data/primitive_polynomials_representations.csv"
    };

    return std::pair<uInteger,uInteger>(degrees[n-1],representations[n-1]);
}

std::vector<uInteger> bin_vector(uInteger num, size_type m){
    std::vector<uInteger> res(m);
    for(int i = 0; i<m; ++i){
        res[m-i-1] = num % 2;
        num = num >> 1;
    }
    return res;
}

uInteger xor_prod_reduce(std::vector<uInteger> a, std::vector<uInteger> b){
    uInteger res = 0;
    size_type n = a.size();
    for (int i = 0; i<n; ++i){
        res ^= a[i]*b[i];
    }
    return res;
}

GeneratingMatrix  SobolNet::generatingMatrix(uInteger coord, Modulus m, std::vector<uInteger> directionNumbers){

    std::vector<std::vector<short>> tmp(m,std::vector<short>(m,0));

    for(uInteger k = 0; k<m; ++k){
        tmp[k][k] = 1;
    }

    if (coord==1)
    {
        return tmp;
    }

    PrimitivePolynomial p = SobolNet::nthPrimitivePolynomial(coord-1);
    auto degree = p.first;
    auto poly_rep = p.second;


    std::vector<uInteger> a = bin_vector(poly_rep,degree-1);
    a.push_back(1);

    for(uInteger i = 0; i<degree; ++i){
        a[i] *= 2 << i;
    }

    for(uInteger k = 0; k < std::min(degree,m); ++k){
        auto dirNum = bin_vector(directionNumbers[k],k+1);

        for(uInteger i = 0; i<k; ++i){
            tmp[i][k] = dirNum[i];
        }
    }

    std::vector<uInteger> reg(directionNumbers.size());
    std::reverse_copy(directionNumbers.begin(),directionNumbers.end(), reg.begin());

    for(uInteger k = degree; k<m; ++k){
        uInteger new_num = xor_prod_reduce(a,reg) ^ reg[degree-1];
        reg.pop_back();
        reg.insert(reg.begin(),new_num);
        auto dirNum = bin_vector(new_num,k+1);
        for(uInteger i = 0; i<k; ++i){
            tmp[i][k] = dirNum[i];
        }
    }
    return tmp;
}

GeneratingMatrix SobolNet::generatingMatrix(size_type dim) const {
    return SobolNet::generatingMatrix(dim,modulus(),m_directionNumbers[dim-1]);
}

std::vector<GeneratingMatrix> SobolNet::generatingMatrices() const {
    std::vector<GeneratingMatrix> res;
    for(int j = 0; j<dimension(); ++j){
        res.push_back(SobolNet::generatingMatrix(j+1,modulus(),m_directionNumbers[j]));
    }
    return res;
}
