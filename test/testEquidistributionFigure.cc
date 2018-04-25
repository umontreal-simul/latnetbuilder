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
#include "netbuilder/EquidistributionFigureOfMerit.h"
#include "netbuilder/GaussMethod.h"
#include "netbuilder/SchmidMethod.h"
#include "netbuilder/DigitalNet.h"

#include "latbuilder/Functor/binary.h"

#include "latcommon/Weights.h"
#include "latcommon/UniformWeights.h"
#include "latcommon/OrderDependentWeights.h"

#include <iostream>

using namespace NetBuilder;
using Coordinates = LatCommon::Coordinates;
using Weights = LatCommon::Weights;

int main(int argc, const char *argv[])
{
    unsigned int s = 10;
    unsigned int m = 15;

    std::unique_ptr<LatCommon::OrderDependentWeights> weights( new LatCommon::OrderDependentWeights(0));
    weights->setWeightForOrder(s,1);
    EquidistributionFigureOfMerit<GaussMethod, LatBuilder::Functor::Max> fig(std::move(weights), s);
    auto figEval = fig.evaluator();

    const DigitalNet& net = DigitalNetBase<NetConstruction::SOBOL>(s, m, m);

    Real acc = 0;
    for (unsigned int dim = 1; dim <= s; ++dim){
        acc = figEval(net,dim,acc);
    }

    std::cout << "Figure of merit: " << acc << std::endl;

    return 0;
}
