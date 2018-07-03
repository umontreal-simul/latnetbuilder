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

#include "netbuilder/FigureOfMerit/TValue.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"
#include "netbuilder/JoeKuo.h"
#include "latticetester/OrderDependentWeights.h"

#include "latbuilder/LFSR258.h"

#include <cstdlib>
#include <chrono>

using namespace NetBuilder;

int main(int argc, char** argv)
{
    Dimension s_max = std::atol(argv[1]);
    unsigned int k_max = std::atoi(argv[2]);
    unsigned int r_max = std::atoi(argv[3]);

    typename NetConstructionTraits<NetConstruction::SOBOL>::RandomGenValueGenerator<LatBuilder::LFSR258> randomGen(0);

    auto tValueDMFig = std::make_unique<FigureOfMerit::TValue<EmbeddingType::UNILEVEL>>();
    auto evalDM = tValueDMFig->evaluator();


    std::cout << "s" << "\t" << "k" << "\t" << "tValueDM" << "\t" << "timeDM" << "\t" << "tValueGauss" << "\t" << "timeGauss" << std::endl;
    for(Dimension s = 1; s <= s_max; ++s)
    {
        auto weights = std::make_unique<LatticeTester::OrderDependentWeights>();
        weights->setWeightForOrder(s, 1);
        auto projDep = std::make_unique<FigureOfMerit::TValueProjMerit<EmbeddingType::UNILEVEL>>(s);
        auto tValueGaussFig = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<EmbeddingType::UNILEVEL>>>(std::numeric_limits<Real>::infinity(), std::move(weights), std::move(projDep));
        auto evalGauss = tValueGaussFig->evaluator();

        for(unsigned int k = 1; k <= k_max; ++k)
        {
            for(unsigned int r = 1; r <= r_max; ++r)
            {
                std::vector<typename NetConstructionTraits<NetConstruction::SOBOL>::GenValue> genValues;
                for(unsigned int coord = 0; coord < s; ++coord)
                {
                    genValues.push_back(randomGen(coord));
                }
                auto net = DigitalNetConstruction<NetConstruction::SOBOL>(s, k, std::move(genValues));
                
                auto t0DM = std::chrono::high_resolution_clock::now();
                uInteger tValueDM = (*evalDM)(net);
                auto t1DM = std::chrono::high_resolution_clock::now();
                evalDM->reset();

                auto t0Gauss = std::chrono::high_resolution_clock::now();
                uInteger tValueGauss = (*evalGauss)(net);
                auto t1Gauss = std::chrono::high_resolution_clock::now();
                evalGauss->reset();

                double timeDM = std::chrono::duration_cast<std::chrono::duration<double>>(t1DM - t0DM).count();
                double timeGauss = std::chrono::duration_cast<std::chrono::duration<double>>(t1Gauss - t0Gauss).count();

                std::cout << s << "\t" << k << "\t" << tValueDM << "\t" << timeDM << "\t" << tValueGauss << "\t" << timeGauss << std::endl;
            }
        }
    }
}
