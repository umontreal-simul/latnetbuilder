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

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/NetConstructionTraits.h"

#include "netbuilder/Task/Eval.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/UniformityProperties.h"

#include <iostream>

using namespace NetBuilder;

int main(int argc, const char *argv[])
{
    unsigned int s = 1120;
    unsigned int m = 1;

    auto net = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(s,m);

    // for(unsigned int d = 1; d <= s; ++d)
    // {
    //     std::cout << *(net->pointerToGeneratingMatrix(d)) << std::endl;
    // }

    // net->extendSize(10,10);

    // for(unsigned int d = 1; d <= s; ++d)
    // {
    //     std::cout << *(net->pointerToGeneratingMatrix(d)) << std::endl;
    // }
    // std::cout << std::endl;

    auto fig = std::make_unique<FigureOfMerit::AProperty>();

    Task::Eval task(std::move(net), std::move(fig), 2);

    task.execute();
    
    return 0;
}
