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

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"
#include "netbuilder/FigureOfMerit/CombinedFigureOfMerit.h"

#include "latcommon/Weights.h"
#include "latcommon/UniformWeights.h"
#include "latcommon/CoordinateSets.h"

#include <iostream>

using namespace NetBuilder;

int main(int argc, const char *argv[])
{
    unsigned int s = 10;
    unsigned int m = 20;

    const DigitalNet& net = DigitalNetConstruction<NetConstruction::SOBOL>(s, m, m);


    auto weights1 = std::make_unique<LatCommon::UniformWeights>(1);

    auto projDep1 = std::make_unique<FigureOfMerit::SimpleProjDepMerit<PointSetType::NET>>();

    auto fig1 = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::SimpleProjDepMerit<PointSetType::NET>>>(1, std::move(weights1), std::move(projDep1));


    auto weights2 = std::make_unique<LatCommon::UniformWeights>(1);

    auto projDep2 = std::make_unique<FigureOfMerit::TValueProjMerit<PointSetType::NET>>(5);

    auto  fig2 = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<PointSetType::NET>>>(1, std::move(weights2), std::move(projDep2));

    std::vector<std::unique_ptr<FigureOfMerit::FigureOfMerit>> figs;
    figs.push_back(std::move(fig1));
    figs.push_back(std::move(fig2));

    std::vector<Real> weights = {1,1};

    auto figComb = std::make_unique<FigureOfMerit::CombinedFigureOfMerit>(1, std::move(figs), std::move(weights));

    auto eval = figComb->evaluator();

    double acc = 0;
    for(unsigned int dim = 1; dim  <=s; ++dim)
    {
        acc = (*eval)(net,dim,acc,true);
    }


    // ==========================================


    /*std::unique_ptr<LatCommon::UniformWeights> weights2( new LatCommon::UniformWeights(1));

    std::unique_ptr<TValueProjMerit<PointSetType::NET>> projDep2 = std::make_unique<TValueProjMerit<PointSetType::NET>>(s);

    WeightedFigureOfMerit<TValueProjMerit<PointSetType::NET>> fig2(1, std::move(weights2), std::move(projDep2));

    auto eval2 = fig2.evaluator();

    double acc = 0;
    for(unsigned int dim = 1; dim  <=s; ++dim)
    {
        acc = (*eval2)(net,dim,acc,true);
    }*/

    return 0;
}
