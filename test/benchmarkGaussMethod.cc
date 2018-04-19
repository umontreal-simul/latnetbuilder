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

#include "latbuilder/DigitalNet/SchmidMethod.h"
#include "latbuilder/DigitalNet/GaussMethod.h"
#include "latbuilder/DigitalNet/ExplicitNet.h"
#include "latbuilder/DigitalNet/ComputationScheme.h"
#include "latbuilder/LFSR258.h"
#include <iostream>
#include <vector>
#include <time.h>


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
        acc = std::max(acc,tValue*weight) ;
    }

    static double combine(double partialFigureDim, double figurePreviousDim){
        return std::max(partialFigureDim,figurePreviousDim);
    }
};

int main(int argc, const char *argv[])
{
    assert(argc == 5);

    int m_max = atol(argv[1]);
    assert(m_max >=1 && m_max <=30);

    int s_min = atol(argv[2]);

    int s_max = atol(argv[3]);
    assert(s_min >= 1 && s_max <= 20 && s_min <= s_max);

    int n_test = atol(argv[4]);
    assert(n_test >= 1);

    auto randomGen = LatBuilder::LFSR258();

    std::cout << "s" << "\t" << "m" << "\t" << "gauss" << std::endl;

    for(int s = s_min; s <= s_max; ++s)
    {
            
        auto compGauss = ComputationScheme<tValueWeights,GaussMethod,MaxFigure>(s,tValueWeights(s));
        compGauss.extendUpToDimension(s);

        for(int m = 1; m <= m_max; ++m)
        {

                float total_gauss = 0;

            for (int k=0; k<n_test; k++){

                double tValueGauss = 0;

                auto net = ExplicitNet<2>(s, m, m, randomGen);

                clock_t t1,t2; 
                bool verbose = false;   
                
                t1=clock();
                compGauss.evaluateFigureOfMerit(net,tValueGauss);
                t2=clock();
                float diff2 ((float)t2-(float)t1);
                total_gauss += diff2;
            }
            double m_gauss = total_gauss/ n_test;

            std::cout << s << "\t" << m << "\t" << m_gauss << std::endl;
        }
    }
    return 0;
}
