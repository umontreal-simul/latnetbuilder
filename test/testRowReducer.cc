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
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/ProgressiveRowReducer.h"

#include "netbuilder/FigureOfMerit/EquidistributionProperty.h"
#include <iostream>

using namespace NetBuilder;

int main(int argc, const char *argv[])
{
    unsigned int s = 1112;
    unsigned int m = s;

    const auto& net = DigitalNetConstruction<NetConstruction::SOBOL>(s,m);

    
    // auto rowReducer = ProgressiveRowReducer();

    // GeneratingMatrix block(0,s);

    // for(unsigned int dim = 1; dim <= s; ++dim)
    // {
    //     block.vstack(net.pointerToGeneratingMatrix(dim)->subMatrix(1,s));
    // }

    // rowReducer.reduceNewBlock(block);

    // std::cout << rowReducer.matrix() << std::endl;

    // std::cout << rowReducer.rowOperations() << std::endl;

    // for(int i: rowReducer.m_permutation)
    // {
    //     std::cout << i << " ";
    // }

    // std::cout << std::endl << std::endl;

    auto fig = FigureOfMerit::AProperty();

    auto eval = fig.evaluator();

    (*eval)(net,true);
   




    
    return 0;
}
