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

#include "latbuilder/DigitalNet/ComputationScheme.h"
#include "latbuilder/DigitalNet/GaussMethod.h"
#include "latbuilder/DigitalNet/SchmidMethod.h"
#include "latbuilder/DigitalNet/ExplicitNet.h"
#include "latbuilder/DigitalNet/SobolNet.h"
#include <iostream>
#include <limits>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "latbuilder/LFSR258.h"

using namespace LatBuilder::DigitalNet;


typedef boost::dynamic_bitset<> projection;
typedef LatBuilder::uInteger uInteger;

class JoeKuoWeights{
    public:
        double operator()(const projection& projRep)
        { 
            if (projRep.count() != 2)
            {
                return 0;
            }
            else
            {
                unsigned int j = 1;
                while(!projRep[j-1])
                {
                    ++j;
                }
                return std::pow(.9999,j-1);
            }
        }
};

struct MaxFigure{

    static void updateFigure(double& acc, int tValue, double weight){
        acc = std::max(acc,tValue*weight);
    }

    static double combine(double partialFigureDim, double figurePreviousDim){
        return std::max(partialFigureDim,figurePreviousDim);
    }
};


typedef typename ExplicitNet<2>::GeneratingMatrix GeneratingMatrix;

int main(int argc, const char *argv[])
{
    int s = 30;
    int m = 20;

    int N = 150;

    LatBuilder::LFSR258 randomGen = LatBuilder::LFSR258();

    int maxCardinal = 2;

    auto comp =  ComputationScheme<JoeKuoWeights,GaussMethod,MaxFigure>(maxCardinal,JoeKuoWeights());

    double bestFigure = 0;
    ExplicitNet<2> bestNet = ExplicitNet<2>(m,m);

    for(unsigned int dim = 1; dim <= s; ++dim)
    {
        ExplicitNet<2> currentNet = bestNet;
        double bestPartialFigure = std::numeric_limits<double>::max();
        for(unsigned int i = 0; i < N; ++i)
        {
            GeneratingMatrix newMat = GeneratingMatrix::randomMatrix(m,m,randomGen);
            currentNet.extendDimension(newMat);
            if (comp.computePartialFigureOfMerit(currentNet, dim, bestPartialFigure, bestFigure))
            {
                if (i >= 1){
                    bestNet.shrinkDimension();
                }
                bestNet.extendDimension(newMat);    
            }
            currentNet.shrinkDimension();
        }
        bestFigure = MaxFigure::combine(bestPartialFigure,bestFigure);
        if (dim < s)
        {
            comp.extend();
        }    
    }

    std::cout << bestFigure << std::endl;

    auto test = SobolNet(m,s);
    double joeKuoFigure = 0;
    comp.evaluateFigureOfMerit(test,joeKuoFigure);

    std::cout << joeKuoFigure << std::endl;

    return 0;
}
