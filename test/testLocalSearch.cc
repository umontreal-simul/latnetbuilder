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

#include "netbuilder/Task/CBCSearch.h"
#include "netbuilder/Task/RandomCBCExplorer.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"

#include "latticetester/Weights.h"
#include "latticetester/UniformWeights.h"

#include <iostream>
#include <limits>
#include <vector>
#include <time.h>

using namespace NetBuilder;
using namespace FigureOfMerit;

int main(int argc, const char *argv[])
{
    unsigned int s = 10;
    unsigned int m = 20;

    int verbose = 0;
    typename NetConstructionTraits<NetConstruction::SOBOL>::DesignParameter designParam(m);
 
    // std::vector<GeneratingMatrix> generatingMatrices;
    // for (int i=1; i<=s; i++){
    //     generatingMatrices.push_back(GeneratingMatrix(m, m));
    // }

    srand (time(NULL));
    int c = 0;
    
    // double bestMerit = 1000000;

    // typename NetConstructionTraits<NetConstruction::SOBOL>::DesignParameter designParam(m);

    unsigned int r = 1;
    auto weights = std::make_unique<LatticeTester::UniformWeights>(1);

    // auto projDep = std::make_unique<ResolutionGapProjMerit<EmbeddingType::MULTILEVEL>>(3,combiner);
    auto projDep = std::make_unique<TValueProjMerit<EmbeddingType::UNILEVEL>>(3);

    auto fig = std::make_unique<WeightedFigureOfMerit<TValueProjMerit<EmbeddingType::UNILEVEL>>>(1, std::move(weights), std::move(projDep));

    auto explorer = std::make_unique<Task::RandomCBCExplorer<NetConstruction::SOBOL>>(s,designParam,r);
    auto search = Task::CBCSearch<NetConstruction::SOBOL,Task::RandomCBCExplorer>(s,designParam, std::move(fig), std::move(explorer), verbose);

    std::cout << "Search design parameter: " << search.designParameter() << std::endl;

    search.onNetSelected().connect( [] (const Task::Search<NetConstruction::SOBOL>& s) { std::cout << "Finished." << std::endl;} );
    search.onFailedSearch().connect( [] (const Task::Search<NetConstruction::SOBOL>& s) { std::cout << "Search failed." <<  std::endl;} );
    search.execute();
    // std::cout << search.outputNet(OutputFormat::GUI) << std::endl;
    std::cout << search.outputMeritValue() << std::endl;

    // typename NetConstructionTraits<NetConstruction::EXPLICIT>::DesignParameter designParamExplicit(std::pair<unsigned int, unsigned int>(m, m));

    auto net = search.bestNet();
    auto bestMerit = search.bestMeritValue();
    std::vector<GeneratingMatrix> generatingMatrices;
    for (int i=1; i<=s; i++){
        generatingMatrices.push_back(net.generatingMatrix(i));
    }

    typename NetConstructionTraits<NetConstruction::EXPLICIT>::DesignParameter designParamExplicit(std::pair<unsigned int, unsigned int>(m, m));
    // for (int k=0; k<s; k++){
    //     for (int i=0; i<m; i++){
    //         for (int j=0; j<m; j++){
    //             if (rand() % 2 == 1){
    //                 generatingMatrices[k].flip(i, j);
    //             } 
    //         }
    //     }
    // }

    auto netExpl = std::make_unique<DigitalNetConstruction<NetConstruction::EXPLICIT>>(s, designParamExplicit, generatingMatrices);

    auto weights2 = std::make_unique<LatticeTester::UniformWeights>(1);
    auto projDep2 = std::make_unique<TValueProjMerit<EmbeddingType::UNILEVEL>>(3);
    auto fig2 = std::make_unique<WeightedFigureOfMerit<TValueProjMerit<EmbeddingType::UNILEVEL>>>(1, std::move(weights2), std::move(projDep2));
    
    auto eval = Task::Eval(std::move(netExpl), std::move(fig2), verbose);
    // for (int i=12; i<m; i++){
    
    int nTries = 400;
    for (int n=0; n<nTries; n++){
        // for (int k=7; k<s; k++){
        int k = rand() % s;  
        // int k = 9;
        int iMin = 12 + (rand() % 8);
        // int j = rand() % m;
        // for (int i=0; i<m; i++){
            // for (int k=0; k<s; k++){
                c++;
                std::vector<GeneratingMatrix> generatingMatrices2 = generatingMatrices;
                // for (int i=iMin; i<m; i++){
                    for (int j=0; j<m; j++){
                        // if (rand() % ((m-1-i) + 2) == 1){
                        if (rand() % 2 == 1){
                            generatingMatrices2[k].flip(iMin, j);
                        } 
                    }
                
                
                auto netExpl = std::make_unique<DigitalNetConstruction<NetConstruction::EXPLICIT>>(s, designParamExplicit, generatingMatrices2);
                eval.reset(std::move(netExpl));
                eval.execute();

                std::cout << "eval " << c << ", value = " << eval.outputMeritValue() << std::endl;
                if (eval.meritValue() <= bestMerit){
                    bestMerit = eval.meritValue();
                    generatingMatrices =  generatingMatrices2;
                }
                
            
        // }
    }

    // for (auto mat: generatingMatrices){
    //     std::cout << mat << std::endl;
    // }
    std::cout << "best merit: " << bestMerit << std::endl;
    
    return 0;
}
