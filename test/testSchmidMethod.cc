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

#include "latbuilder/DigitalNet/DigitalUtil.h"
#include "latbuilder/DigitalNet/SchmidMethod.h"
#include "latbuilder/DigitalNet/DigitalNet.h"
#include "latbuilder/DigitalNet/SobolNet.h"
#include "latbuilder/DigitalNet/ComputationScheme.h"
#include <iostream>
#include <vector>
#include <limits>

using namespace LatBuilder::DigitalNet;

typedef LatBuilder::uInteger uInteger;
typedef boost::dynamic_bitset<> projection;

class dummyWeights{
    public:
        float operator()(const projection& projRep){ return rand() % 1000;; }
};

struct MaxFigure{
    static void updateFigure(double& acc, int tValue, double weight){
        acc = std::max(acc,tValue*weight);
    }
};

template<typename DERIVED,typename WEIGHTS,typename COMPUTATION_METHOD, typename FIGURE_OF_MERIT>
double computeFigureOFMerit(const DigitalNet<DERIVED,2>& net, int maximalCardinality, WEIGHTS weights)
{

}

int main(int argc, const char *argv[])
{
    std::vector<std::vector<uInteger>> directionNumbers = {{},{1},{1,3},{1,3,1},{1,1,1},{1,1,3,3}};
    int m = 15;
    int s = 6;
    auto test = SobolNet(m,s,directionNumbers);

    int maximalCardinality = 3;

    unsigned int dim = test.dimension();
    double fig = 0;
    for(unsigned lastDimension = 2; lastDimension <=dim; ++lastDimension)
    {
        auto comp = ComputationScheme<dummyWeights, SchmidMethod, MaxFigure>(lastDimension,maximalCardinality,dummyWeights());
        comp.evaluateFigureOfMerit(test,fig);
    }
    std::cout << fig << std::endl;


    /* auto gen1 = test.generatingMatrix(1);
    auto gen2 = test.generatingMatrix(2);
    auto gen3 = test.generatingMatrix(3);
    auto gen4 = test.generatingMatrix(4);
    auto gen5 = test.generatingMatrix(5);
    auto gen6 = test.generatingMatrix(6);

    auto comp1 = ComputationScheme<dummyWeights,SchmidMethod,MaxFigure>(2,2,dummyWeights());

    DigitalNet<SobolNet,2>* bestNet = 0;

    double optimalFigureOfMerit = std::numeric_limits<double>::max();

    std::cout << (0 < optimalFigureOfMerit) << std::endl;

    comp1.computeFigureOfMerit(test,bestNet,optimalFigureOfMerit);
    std::cout << optimalFigureOfMerit << std::endl;

    optimalFigureOfMerit = std::numeric_limits<double>::max();
    auto comp2 = ComputationScheme<dummyWeights,SchmidMethod,MaxFigure>(3,2,dummyWeights(),comp1);
    comp2.computeFigureOfMerit(test,bestNet,optimalFigureOfMerit);
    std::cout << optimalFigureOfMerit << std::endl;
 */
/*  std::cout << SchmidMethod::computeTValue({gen1,gen2},0) << std::endl;
    std::cout << SchmidMethod::computeTValue({gen1,gen3},0) << std::endl;
    std::cout << SchmidMethod::computeTValue({gen2,gen3},0) << std::endl;
    std::cout << SchmidMethod::computeTValue({gen1,gen4},0) << std::endl;
    std::cout << SchmidMethod::computeTValue({gen2,gen4},0) << std::endl;
    std::cout << SchmidMethod::computeTValue({gen3,gen4},0) << std::endl;
    std::cout << SchmidMethod::computeTValue({gen1,gen2,gen3},1) << std::endl;
    std::cout << SchmidMethod::computeTValue({gen1,gen2,gen3,gen4},2) << std::endl; */

    return 0;
}
