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

#include "netbuilder/Types.h"
#include "netbuilder/DigitalNet.h"

#include <iostream>

using namespace NetBuilder;

int main(int argc, const char *argv[])
{
    unsigned int s = 10;
    unsigned int m = 5;

    /*DigitalNet* test = new DigitalNetConstruction<NetConstruction::SOBOL>(s,m,m);

    for(unsigned int j = 1; j <=s; ++j)
    {
        std::cout << test->generatingMatrix(j) << std::endl;
    }

    delete test ; */

    /*auto test = DigitalNetConstruction<NetConstruction::UNIRANDOM>(s,m,m);

    for(unsigned int j = 1; j <=s; ++j)
    {
        std::cout << test.generatingMatrix(j) << std::endl;
    }

    test = test.extendSize(m);

    for(unsigned int j = 1; j <=s; ++j)
    {
        std::cout << test.generatingMatrix(j) << std::endl;
    }*/

    for(unsigned int dim = 1; dim < 10; ++dim)
    {
        auto foo = DigitalNetConstruction<NetConstruction::SOBOL>::ConstructionMethod::genValueSpaceDim(dim);

        std::cout << foo.size() << std::endl; 
    }

    return 0;
}
