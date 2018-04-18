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

class dummyWeights{
    public:
        float operator()(const projection& projRep){ return rand() % 1000;; }
};

struct MaxFigure{
    static void updateFigure(double& acc, int tValue, double weight){
        acc = std::max(acc,tValue*weight);
    }
};

int main(int argc, const char *argv[])
{
    std::vector<std::vector<uInteger>> directionNumbers = {{},{1},{1,3},{1,3,1},{1,1,1},{1,1,3,3}};
    int m = 10;
    int s = 6;
    auto test = SobolNet(m,s,directionNumbers);

    int maximalCardinality = s;

    auto comp = ComputationScheme<tValueWeights,SchmidMethod,MaxFigure>(maximalCardinality,tValueWeights(s));
    comp.extend(s-1);

    double tValue = 0;
    comp.evaluateFigureOfMerit(test,tValue);

    std::cout << "t-value: " << tValue << std::endl;
    return 0;
}
