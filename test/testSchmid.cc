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
#include "netbuilder/LevelCombiner.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/CombinedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/UniformityProperties.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"
#include "netbuilder/JoeKuo.h"

#include "netbuilder/Task/Eval.h"

#include <iostream>
#include <memory>
#include <limits>
#include <cstdlib>



using namespace NetBuilder;

int main(int argc, const char *argv[])
{
    unsigned int m = 10 ;
    unsigned int s = 10;

    Real meritGauss = -1;
    Real meritSchmid = -1;

    // Gauss step

    {
        auto weights = std::make_unique<NetBuilder::JoeKuo::Weights>();
        auto fig1 = std::make_unique<FigureOfMerit::AProperty>();

        auto projDep = std::make_unique<FigureOfMerit::TValueProjMerit<PointSetType::MULTILEVEL>>(2, JoeKuo::Combiner<6>());

        auto fig2 = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<PointSetType::MULTILEVEL>>>(std::numeric_limits<Real>::infinity(), std::move(weights), std::move(projDep));

        std::vector<std::unique_ptr<FigureOfMerit::FigureOfMerit>> figures;
    

        figures.push_back(std::move(fig1));
        figures.push_back(std::move(fig2));


        std::vector<Real> weightsFigure {1,1};

        auto fig = std::make_unique<FigureOfMerit::CombinedFigureOfMerit>(std::numeric_limits<Real>::infinity(),std::move(figures),weightsFigure);
        
        auto baseNet = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(s,m);
    
        
        auto task = std::make_unique<Task::Eval>(std::move(baseNet), std::move(fig), 0);
        
        task->execute();

        meritGauss = task->outputMeritValue() ;

    }

    // Schmid step

    {
        auto weights = std::make_unique<NetBuilder::JoeKuo::Weights>();
        auto fig1 = std::make_unique<FigureOfMerit::AProperty>();

        auto projDep = std::make_unique<FigureOfMerit::TValueProjMerit<PointSetType::MULTILEVEL, SchmidMethod>>(2, JoeKuo::Combiner<6>());

        auto fig2 = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<PointSetType::MULTILEVEL, SchmidMethod>>>(std::numeric_limits<Real>::infinity(), std::move(weights), std::move(projDep));

        std::vector<std::unique_ptr<FigureOfMerit::FigureOfMerit>> figures;
    

        figures.push_back(std::move(fig1));
        figures.push_back(std::move(fig2));


        std::vector<Real> weightsFigure {1,1};

        auto fig = std::make_unique<FigureOfMerit::CombinedFigureOfMerit>(std::numeric_limits<Real>::infinity(),std::move(figures),weightsFigure);
        
        auto baseNet = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(s,m);
    
        
        auto task = std::make_unique<Task::Eval>(std::move(baseNet), std::move(fig), 0);
        
        task->execute();

        meritSchmid = task->outputMeritValue() ;

    }

    std::cout <<"Dimension " << s << ": " << "Schmid:" << meritSchmid << " / " << "Gauss: " << meritGauss << std::endl;

    return 0;
}
