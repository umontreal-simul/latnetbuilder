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

#include "netbuilder/Task/Eval.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"
#include "netbuilder/FigureOfMerit/CombinedFigureOfMerit.h"

#include "latcommon/Weights.h"
#include "latcommon/UniformWeights.h"

#include <iostream>
#include <limits>

using namespace NetBuilder;

int main(int argc, const char *argv[])
{
    unsigned int s = 20;
    unsigned int m = 30;

    //unsigned int r = 100;
    auto weights = std::make_unique<LatCommon::UniformWeights>(1);

    auto combiner = [] (std::vector<unsigned int> merits) -> Real 
    { 
        unsigned int m = merits.size();
        Real res;
        for(double t : merits)
        {
            res = std::max(res, intPow(t,6)/(m-t+1));
        }
        return res;
    };

    auto projDep = std::make_unique<FigureOfMerit::TValueProjMerit<NetEmbed::EMBEDDED>>(2,combiner);

    auto fig = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<NetEmbed::EMBEDDED>>>(1, std::move(weights), std::move(projDep), OpMax());

    // auto projDep = std::make_unique<FigureOfMerit::TValueProjMerit<NetEmbed::SIMPLE>>(3);

    // auto fig = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<NetEmbed::SIMPLE>>>(1, std::move(weights), std::move(projDep), OpMax());

    auto net = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(s,m,m);

    auto task = Task::Eval(std::move(net),std::move(fig));

    task.execute();

    std::cout << task.meritValue() << std::endl;

    return 0;
}
