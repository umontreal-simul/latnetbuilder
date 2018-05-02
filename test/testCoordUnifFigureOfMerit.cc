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

#include <NTL/GF2X.h>

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/CoordUniformFigureOfMerit.h"

#include "latbuilder/Kernel/PAlpha.h"


#include "latcommon/Weights.h"
#include "latcommon/UniformWeights.h"
#include "latcommon/CoordinateSets.h"

#include <iostream>

using namespace NetBuilder;
// using namespace LatBuilder;

int main(int argc, const char *argv[])
{
    unsigned int s = 10;
    unsigned int m = 20;

    const DigitalNet& net = DigitalNetConstruction<NetConstruction::SOBOL>(s, m, m);


    auto weights1 = std::make_unique<LatCommon::UniformWeights>(1);

    auto kernel = LatBuilder::Kernel::PAlpha(2);

    auto fig1 = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlpha>>(std::move(weights1), m, kernel);
    // auto eval = fig1->evaluator();

    std::cout << *fig1 << std::endl;


    return 0;
}

// int main(){
//     return 0;
// }
