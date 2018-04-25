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
#include "netbuilder/EquidistributionFigureOfMerit.h"
#include "netbuilder/GaussMethod.h"
#include "netbuilder/SchmidMethod.h"
#include "netbuilder/DigitalNet.h"

#include "latbuilder/Functor/binary.h"

#include "latcommon/Weights.h"
#include "latcommon/UniformWeights.h"
#include "latcommon/OrderDependentWeights.h"

#include <boost/signals2.hpp>
#include <iostream>


using namespace NetBuilder;
using Coordinates = LatCommon::Coordinates;
using Weights = LatCommon::Weights;


typedef LatCommon::Coordinates Coordinates;
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

    std::cout << "s" << "\t" << "m" << "\t" << "schmid" << "\t" << "reversed_schmid" << "\t" << "gauss" << std::endl;

    for(int s = s_min; s <= s_max; ++s)
    {
        EquidistributionFigureOfMerit<GaussMethod, LatBuilder::Functor::Max> figGauss(std::unique_ptr<Weights>(new LatCommon::OrderDependentWeights(1)), s);
        auto gaussEval = figGauss.evaluator();

        EquidistributionFigureOfMerit<SchmidMethod, LatBuilder::Functor::Max> figSchmid(std::unique_ptr<Weights>(new LatCommon::OrderDependentWeights(1)), s);
        auto schmidEval = figSchmid.evaluator();

        EquidistributionFigureOfMerit<ReversedSchmidMethod, LatBuilder::Functor::Max> figReversedSchmid(std::unique_ptr<Weights>(new LatCommon::OrderDependentWeights(1)), s);
        auto reversedSchmidEval = figReversedSchmid.evaluator();

        for(int m = 1; m <= m_max; ++m)
        {

                float total_schmid = 0;
                float total_gauss = 0;
                float total_reversed_schmid = 0;

            for (int k=0; k<n_test; k++){

                double tValueSchmid = 0;
                double tValueGauss = 0;
                double tValueReversedSchmid = 0;


                const DigitalNet& net = DigitalNetBase<NetConstruction::UNIRANDOM>(s, m, m);

                clock_t t1,t2, t3, t4, t5, t6;  
                bool verbose = false;  

                t1=clock();
                for (unsigned int dim = 1; dim <= s; ++dim)
                {
                    tValueSchmid = schmidEval(net, dim, tValueSchmid, true);
                }
                t2=clock();
                float diff ((float)t2-(float)t1);
                total_schmid += diff;

                t3=clock();
                for (unsigned int dim = 1; dim <= s; ++dim)
                {
                    tValueGauss = gaussEval(net,dim,tValueGauss, false);
                }
                t4=clock();
                float diff2 ((float)t4-(float)t3);
                total_gauss += diff2;

                t5=clock();
                for (unsigned int dim = 1; dim <= s; ++dim)
                {
                    tValueReversedSchmid = reversedSchmidEval(net,dim,tValueReversedSchmid, false);
                }
                t6=clock();
                float diff3 ((float)t6-(float)t5);
                total_reversed_schmid += diff3;
            }

            double m_schmid =  total_schmid/n_test;
            double m_reversed_schmid = total_reversed_schmid/n_test;
            double m_gauss = total_gauss/ n_test;

            std::cout << s << "\t" << m << "\t" << m_schmid << "\t" << m_reversed_schmid << "\t" << m_gauss << std::endl;
        }
    }

    return 0;
}
