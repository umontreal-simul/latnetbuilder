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

using LatBuilder::uInteger; 
typedef SobolNet::GeneratingMatrix GeneratingMatrix;

SobolNet::PrimitivePolynomial SobolNet::nthPrimitivePolynomial(uInteger n){
    // primitive polynomials are hard-coded because their computation is really complex.
    constexpr int degrees[] =
    {
        #include "data/primitive_polynomials_degrees.csv"
    };

    constexpr unsigned int representations[] =
    {
        #include "data/primitive_polynomials_representations.csv"
    };
    if (n>0 && n <= 21200)
    return std::pair<int,uInteger>(degrees[n-1],representations[n-1]);
    else{
        return std::pair<int,uInteger>(0,0);
    }
}

/** Compute the m-bit binary representation of the given integer. The most significant bit is the leftest non zero
 * bit in the returned vector.
 * @param num non-negative integer
 * @param size_type size of the binary representation
 */
std::vector<uInteger> bin_vector(uInteger num, size_type m){
    std::vector<uInteger> res(m);
    for(int i = 0; i<m; ++i){
        res[m-i-1] = num % 2;
        num = num >> 1;
    }
    return res;
}

/** Compute the element-wise product of two vector and reduce the resulting vector using the exclusive or operator.
 * @param a first vector
 * @param b second vector 
 */ 
uInteger xor_prod_reduce(std::vector<uInteger> a, std::vector<uInteger> b){
    uInteger res = 0;
    size_type n = a.size();
    for (int i = 0; i<n; ++i){
        res ^= a[i]*b[i];
    }
    return res;
}

GeneratingMatrix SobolNet::generatingMatrix(size_type coord) const {

    int m = modulus(); // size of the generating matrix
    const std::vector<uInteger>& directionNumbers = m_directionNumbers[coord]; // direction number for the given coordinate

    GeneratingMatrix tmp(m,std::vector<short>(m,0));

    for(uInteger k = 0; k<m; ++k){
        tmp[k][k] = 1; // start with identity
    }

    if (coord==1) // special case for the first dimension
    {
        return tmp;
    }

    // compute the vector defining the linear recurrence on the columns of the matrix

    PrimitivePolynomial p = SobolNet::nthPrimitivePolynomial(coord-1);
    auto degree = p.first;
    auto poly_rep = p.second;

    std::vector<uInteger> a = bin_vector(poly_rep,degree-1);
    a.push_back(1);

    for(uInteger i = 0; i<degree; ++i){
        a[i] *= 2 << i;
    }

    // initialization of the first columns

    for(uInteger k = 0; k < std::min(degree,m); ++k){
        auto dirNum = bin_vector(directionNumbers[k],k+1);

        for(uInteger i = 0; i<k; ++i){
            tmp[i][k] = dirNum[i];
        }
    }

    std::vector<uInteger> reg(directionNumbers.size()); // register for the linear reccurence
    std::reverse_copy(directionNumbers.begin(),directionNumbers.end(), reg.begin()); // should be reversed

    // computation of the recurrence
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

std::vector<GeneratingMatrix> SobolNet::generatingMatrices() const {
    std::vector<GeneratingMatrix> res;
    for(int j = 0; j<dimension(); ++j){
        res.push_back(generatingMatrix(j+1));
    }
    return res;
}

