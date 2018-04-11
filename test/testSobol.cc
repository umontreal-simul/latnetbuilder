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

#include "latbuilder/DigitalNet/DigitalNet.h"
#include "latbuilder/DigitalNet/SobolNet.h"
#include "latbuilder/Types.h"

using namespace LatBuilder::DigitalNet;

typedef LatBuilder::uInteger uInteger;

int main(int argc, const char *argv[])
{
    std::vector<std::vector<uInteger>> directionNumbers = {{},{1},{1,3},{1,3,1},{1,1,1},{1,1,3,3}};
    Modulus m = 10;
    uInteger s = 6;
    auto test = SobolNet(m,s,directionNumbers);

/*     auto res = test.generatingMatrices();

    for (const auto &mat : res)
    {
        for ( const auto &row : mat )
        {
            for ( const auto &s : row ) std::cout << s << ' ';
            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    for (int d = 2; d<=10; ++d)
    {
        auto p = SobolNet::nthPrimitivePolynomial(d-1);
        auto degree = p.first;
        auto rep = p.second;
        std::cout << degree << " " << rep << std::endl;
    } */

    std::vector<std::vector<uInteger>> res = rolledGeneratingMatrices<SobolNet>(test);

    for (int i = 0; i< test.dimension(); ++i)
    {
        std::cout << "coord: " << i+1 << " ";
        for(const auto &row : res[i])
        {
            std::cout << row << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}

