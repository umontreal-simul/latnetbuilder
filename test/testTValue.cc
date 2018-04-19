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
#include "latbuilder/DigitalNet/GaussMethod.h"
#include "latbuilder/DigitalNet/DigitalNet.h"
#include "latbuilder/DigitalNet/SobolNet.h"
#include "latbuilder/DigitalNet/ComputationScheme.h"
#include <iostream>
#include <vector>
#include <map>
#include <limits>

using namespace LatBuilder::DigitalNet;

typedef LatBuilder::uInteger uInteger;
typedef boost::dynamic_bitset<> projection;

class tValueWeights{
    public:
        tValueWeights(unsigned int dimension):
        m_dimension(dimension)
        {};
        float operator()(const projection& projRep)
        { return projRep.size()==m_dimension && projRep.all();; 
        }
    private:
        unsigned int m_dimension;
};

struct MaxFigure{
    static void updateFigure(double& acc, int tValue, double weight){
        acc = std::max(acc,tValue*weight);
    }
};

int main(int argc, const char *argv[])
{

    int m = 10;
    int s = 20;

    auto test = SobolNet(m,s);

    int maximalCardinality = s;

    auto compSchmid = ComputationScheme<tValueWeights,SchmidMethod,MaxFigure>(maximalCardinality,tValueWeights(s));
    compSchmid.extend(s-1);

    auto compReversedSchmid = ComputationScheme<tValueWeights,ReversedSchmidMethod,MaxFigure>(maximalCardinality,tValueWeights(s));
    compReversedSchmid.extend(s-1);

    auto compGauss = ComputationScheme<tValueWeights,GaussMethod,MaxFigure>(maximalCardinality,tValueWeights(s));
    compGauss.extend(s-1);


    double tValueSchmid = 0;
    double tValueReversedSchmid = 0; 
    double tValueGauss = 0;

    compSchmid.evaluateFigureOfMerit(test,tValueSchmid);
    std::cout << "t-value Schmid: " << tValueSchmid << std::endl;

    compReversedSchmid.evaluateFigureOfMerit(test,tValueReversedSchmid);
    std::cout << "t-value Reversed Schmid: " << tValueSchmid << std::endl;

    compGauss.evaluateFigureOfMerit(test,tValueGauss);
    std::cout << "t-value Gauss: " << tValueGauss << std::endl;
    
    return 0;
}
