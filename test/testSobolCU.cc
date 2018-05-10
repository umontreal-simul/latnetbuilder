// // This file is part of Lattice Builder.
// //
// // Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/GeneratingMatrix.h"
#include "netbuilder/NetConstructionTraits.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/CoordUniformFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"

#include "latbuilder/Kernel/PAlphaPLR.h"
#include "latbuilder/Types.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/LFSR258.h"

#include "netbuilder/Task/Eval.h"

#include "latcommon/Weights.h"
#include "latcommon/UniformWeights.h"

#include <iostream>
#include <fstream>
#include <time.h>
#include <math.h>

using namespace NetBuilder;

unsigned int getmsb (unsigned long long x){
    unsigned int r = 0;
    if (x < 1) return 0;
    while (x >>= 1) r++;
    return r;
}

template <NetConstruction NC>
std::vector<std::vector<unsigned long>> points(std::unique_ptr<DigitalNetConstruction<NC>> net, std::vector<uInteger> m_graycode){
    std::vector<std::vector<unsigned long>> points;
    for (int i=0; i<net->numPoints(); i++){
        points.push_back(std::vector<unsigned long>());
    }

    for (int j=1; j <= net->dimension(); j++){
        GeneratingMatrix matrix = net->generatingMatrix(j);
        std::vector<unsigned long> cols = matrix.getColsReverse();
        std::vector<unsigned long> m_permutation = std::vector<unsigned long>(net->numPoints(), 0);
        points[0].push_back(0);
        for (size_type i=1; i<m_permutation.size(); ++i){
            m_permutation[i] = m_permutation[i-1] ^ cols[m_graycode[i-1]];
            points[i].push_back(m_permutation[i]);
        }
    }
    return points;
}

void digital_shift(LatBuilder::LFSR258 gen, std::vector<std::vector<unsigned long>>& points){
    for (int j=0; j<points[0].size(); j++){
        unsigned long delta = gen() % points.size();
        // std::cout << "delta " << j <<": " << delta << std::endl;
        for (int i=0; i<points.size(); i++){
            points[i][j] ^= delta;
        }
    }
}

Real f(std::vector<Real>& x){
    Real y = 0;
    for (int j=0; j<x.size(); j++){
        y += x[j];
    }
    // std::cout << std::endl;
    return sin(M_PI*y);
}

Real integrate(std::vector<std::vector<unsigned long>>& points){
    Real s = 0;
    for (int i=0; i<points.size(); i++){
        std::vector<Real> x = std::vector<Real>(points[0].size(), 0);
        for (int j=0; j<x.size(); j++){
            x[j] = ((Real) points[i][j]) / points.size();
        }
        s += f(x);
    }
    s /= points.size();
    return s;
}

int main(int argc, const char *argv[])
{
    unsigned int s = 2;
    unsigned int m = 18;
    int nTries = 350;
    int nRandomSimul = 400;
    unsigned int modulus = intPow(2, m);

    std::vector<uInteger> m_graycode = std::vector<uInteger>();
    for(size_type i=m_graycode.size(); i < modulus; i++){
          m_graycode.push_back(getmsb(((i >> 1) ^ i)^(((i+1) >> 1) ^ (i+1))));
    }

    auto kernel = LatBuilder::Kernel::PAlphaPLR(2);

    LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, LatBuilder::PointSetType::UNILEVEL> param(intPow(2, m));
    
    typedef NetConstructionTraits<NetConstruction::SOBOL> ConstructionMethod;

    std::ofstream myfile;
    myfile.open ("test_dim2.txt");
    myfile << "s=" << s << endl;
    myfile << "m=" << m << endl;
    myfile << "nTries=" << nTries << endl;


    typedef ConstructionMethod:: template RandomGenValueGenerator <LatBuilder::LFSR258> randomGenValueGenerator;
    randomGenValueGenerator randomGen = randomGenValueGenerator();

    clock_t t1,t2; 
    t1=clock();
    for (int i=0; i<nTries; i++){
        // if (i % 10 == 0){
            std::cout << "Reached " << i << std::endl;
        // }
        
        auto weights1 = std::make_unique<LatCommon::ProductWeights>(0.1);
        auto net = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(1, m, m);
        auto net2 = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(1, m, m);
        auto net3 = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(1, m, m);
        for (int dim=2; dim<=s; dim++){
            net = net->extendDimension(randomGen(dim));
            net2 = net2->extendDimension(randomGen(dim));
            net3 = net3->extendDimension(randomGen(dim));
        }
        // std::cout << "1" << std::endl;
        std::vector<std::vector<unsigned long>> vecPoints = points(std::move(net3), m_graycode);
        // for (int j=0; j<vecPoints.size(); j++){
        //     for (int i=0; i<vecPoints[0].size(); i++){
        //         std::cout << vecPoints[j][i]<< " ";
        //     }
        //     std::cout << std::endl;
        // }
        
        // std::cout << "digital shift done" << std::endl;
        // for (int j=0; j<vecPoints.size(); j++){
        //     for (int i=0; i<vecPoints[0].size(); i++){
        //         std::cout << vecPoints[j][i] << " ";
        //     }
        //     std::cout << std::endl;
        // }
        // std::cout << "2" << std::endl;
        // for(int dim=1; dim<=s; dim++){
        //     myfile << net->generatingMatrix(dim) << std::endl;
        // }
        auto fig1 = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR, LatBuilder::PointSetType::UNILEVEL>>(std::move(weights1), m, param, kernel);
        auto task = Task::Eval(std::move(net),std::move(fig1));
        task.execute();
        myfile << task.meritValue() << std::endl;

        // std::cout << "3" << std::endl;

        auto weights2 = std::make_unique<LatCommon::ProductWeights>(0.1);
        auto projDep2 = std::make_unique<FigureOfMerit::TValueProjMerit<PointSetType::UNILEVEL>>(3);
        auto  fig2 = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<PointSetType::UNILEVEL>>>(1, std::move(weights2), std::move(projDep2));
        auto task2 = Task::Eval(std::move(net2),std::move(fig2));
        task2.execute();
        myfile << task2.meritValue() << std::endl;

        // std::cout << "4" << std::endl;

        LatBuilder::LFSR258 gen = LatBuilder::LFSR258();
        std::vector<Real> int_values = std::vector<Real>(nRandomSimul, 0);
        Real mean = 0;
        Real variance = 0;
        for (int j=0; j<nRandomSimul; j++){
            // if (j % 100 == 0){
            // std::cout << "Reached " << j << std::endl;
        // }
            gen.jump();
            digital_shift(gen, vecPoints);
            int_values[j] = integrate(vecPoints);
            mean += int_values[j] / nRandomSimul;
            variance += int_values[j]*int_values[j] / nRandomSimul;
        }
        variance -= mean*mean;
        myfile << mean << std::endl;
        myfile << sqrt(variance) << std::endl;
        myfile << std::endl;
        // std::cout << "5" << std::endl;
    }
    t2=clock();
    float diff (((float)t2-(float)t1)/nTries);
    std::cout << diff << std::endl;

    myfile.close();



    return 0;
}


    // auto vecGenValues = ConstructionMethod::genValueSpaceDim(s+1);    

    // for (int i=0; i<vecGenValues.size(); i++){
    //     auto weights1 = std::make_unique<LatCommon::OrderDependentWeights>(0.1);
    //     auto net = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(s, m, m);

    //     auto net2 = net->extendDimension(vecGenValues[i]);
    //     std::cout << net2->generatingMatrix(s+1) << std::endl;

    //     auto fig1 = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR, LatBuilder::PointSetType::UNILEVEL>>(std::move(weights1), m, param, kernel);
    //     std::cout << "merit value: ";
    //     auto task = Task::Eval(std::move(net2),std::move(fig1));
    //     task.execute();
    //     std::cout << task.meritValue() << std::endl;
    // }