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
#include "netbuilder/LevelCombiner.h"

#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/Task/Eval.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/CombinedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/UniformityProperties.h"
#include "netbuilder/FigureOfMerit/ResolutionGapProjMerit.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"

#include <iostream>
#include "latticetester/Weights.h"
#include "latticetester/UniformWeights.h"


using namespace NetBuilder;

int main(int argc, const char *argv[])
{
    unsigned int s = 10;
    unsigned int m = 20;

    const auto& net = DigitalNetConstruction<NetConstruction::SOBOL>(s,m);

    auto weights = std::make_unique<LatticeTester::UniformWeights>(1);

    auto fig1 = std::make_unique<FigureOfMerit::AProperty>();

    auto projDep = std::make_unique<FigureOfMerit::TValueProjMerit<PointSetType::MULTILEVEL>>(2, JoeKuoD6Combiner());
    auto fig2 = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<PointSetType::MULTILEVEL>>>(1, std::move(weights), std::move(projDep));

    std::vector<std::unique_ptr<FigureOfMerit::FigureOfMerit>> figures;
    
    figures.push_back(std::move(fig1));
    figures.push_back(std::move(fig2));

    std::vector<Real> weightsFigure {1,1};

    auto fig = FigureOfMerit::CombinedFigureOfMerit(1,std::move(figures),weightsFigure);
    auto eval = fig.evaluator();

    (*eval)(net,true);

    return 0;
}
