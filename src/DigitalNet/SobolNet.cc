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
#include <string>
#include <fstream>
#include <vector>
#include <assert.h>
#include <boost/algorithm/string.hpp>

using namespace LatBuilder::DigitalNet;

using LatBuilder::uInteger; 
typedef typename SobolNet::GeneratingMatrix GeneratingMatrix;

static const std::array<unsigned int,21200> degrees =
{{
    #include "latbuilder/DigitalNet/data/primitive_polynomials_degrees.csv"
}};

static const std::array<unsigned long,21200> representations =
{{
    #include "latbuilder/DigitalNet/data/primitive_polynomials_representations.csv"
}};

SobolNet::PrimitivePolynomial SobolNet::nthPrimitivePolynomial(unsigned int n){
    // primitive polynomials are hard-coded because their computation is really complex.
    if (n>0 && n <= 21200)
    return std::pair<unsigned int,uInteger>(degrees[n-1],representations[n-1]);
    else{
        return std::pair<unsigned int,uInteger>(0,0);
    }
}

/** Compute the m-bit binary representation of the given integer. The most significant bit is the leftest non zero
 * bit in the returned vector.
 * @param num non-negative integer
 * @param size_type size of the binary representation
 */
std::vector<uInteger> bin_vector(uInteger num, size_type m){
    std::vector<uInteger> res(m);
    for(unsigned int i = 0; i<m; ++i){
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
    for (unsigned int i = 0; i<n; ++i){
        res ^= a[i]*b[i];
    }
    return res;
}

GeneratingMatrix SobolNet::generatingMatrix(unsigned int coord) const {

    unsigned int m = modulus(); // size of the generating matrix
    const std::vector<uInteger>& directionNumbers = m_directionNumbers[coord-1]; // direction number for the given coordinate

    GeneratingMatrix tmp(m,m);

    for(unsigned int k = 0; k<m; ++k){
        tmp(k,k) = 1; // start with identity
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

    for(unsigned int i = 0; i<degree; ++i){
        a[i] *= 2 << i;
    }

    // initialization of the first columns

    for(unsigned int k = 0; k < std::min(degree,m); ++k){
        auto dirNum = bin_vector(directionNumbers[k],k+1);

        for(unsigned int i = 0; i<k; ++i){
            tmp(i,k) = dirNum[i];
        }
    }

    std::vector<uInteger> reg(directionNumbers.size()); // register for the linear reccurence
    std::reverse_copy(directionNumbers.begin(),directionNumbers.end(), reg.begin()); // should be reversed

    // computation of the recurrence
    for(unsigned int k = degree; k<m; ++k){
        uInteger new_num = xor_prod_reduce(a,reg) ^ reg[degree-1];
        reg.pop_back();
        reg.insert(reg.begin(),new_num);
        auto dirNum = bin_vector(new_num,k+1);
        for(unsigned int i = 0; i<k; ++i){
            tmp(i,k) = dirNum[i];
        }
    }
    return tmp;
}

std::vector<GeneratingMatrix> SobolNet::generatingMatrices() const {
    std::vector<GeneratingMatrix> res;
    for(unsigned int j = 0; j<dimension(); ++j){
        res.push_back(generatingMatrix(j+1));
    }
    return res;
}

std::vector<std::vector<uInteger>> SobolNet::readJoeKuoDirectionNumbers(unsigned int dimension)
{
    assert(dimension >= 1 && dimension <= 21201);
    std::ifstream file("../data/JoeKuoSobolNets.csv");
    std::vector<std::vector<uInteger>> res(dimension);
    res[0] = {};
    std::string sent;
    for(int i = 2; i <= dimension; ++i)
    {
        if(getline(file,sent))
        {
            std::vector<std::string> fields;
            boost::split( fields, sent, boost::is_any_of( ";" ) );
            for( const auto& token : fields)
            {
                res[i-1].push_back(std::stol(token));
            }
        }
        else
        {
            std::cout << "Aie" << std::endl;
            break;
        }
    }
    return res;
}