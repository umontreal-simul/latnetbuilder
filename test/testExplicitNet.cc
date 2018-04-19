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
#include "latbuilder/DigitalNet/ExplicitNet.h"
#include "latbuilder/Types.h"

using namespace LatBuilder::DigitalNet;

typedef LatBuilder::uInteger uInteger;


int main(int argc, const char *argv[])
{
    std::vector<std::vector<uInteger>> directionNumbers = {{},{1},{1,3},{1,3,1},{1,1,1},{1,1,3,3}};
    int m = 10;
    int s = 6;
    auto foo = SobolNet(m,s,directionNumbers);

    auto test = ExplicitNet<2>(foo.generatingMatrices());

    std::cout << test.generatingMatrix(1) << std::endl;
    std::cout << test.generatingMatrix(2) << std::endl;
    std::cout << test.generatingMatrix(3) << std::endl;
    std::cout << test.generatingMatrix(4) << std::endl;
    std::cout << test.generatingMatrix(5) << std::endl;
    std::cout << test.generatingMatrix(6) << std::endl;

    auto test2 = ExplicitNet<2>(s,m,m);

    auto test3 = ExplicitNet<11>(s,m,m);

    std::cout << test2.generatingMatrix(1) << std::endl;
    std::cout << test3.generatingMatrix(1) << std::endl;

    return 0;
}

