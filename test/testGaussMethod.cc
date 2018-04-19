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
#include "latbuilder/DigitalNet/SobolNet.h"
#include "latbuilder/DigitalNet/ComputationScheme.h"
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
        acc = std::max(acc,tValue*weight);
    }
};

int main(int argc, const char *argv[])
{
    int m = 0;
    for (int k=0; k < 3; k++){
        m *= 10;
        m += (int) (argv[1][k] - '0');
    }

    int s = (int) (argv[2][0] - '0');

    int n_test = 0;
    for (int k=0; k < 3; k++){
        n_test *= 10;
        n_test += (int) (argv[3][k] - '0');
    }

    int random = (int) (argv[4][0] - '0');

    std::cout << m << std::endl;
    std::cout << s << std::endl;
    std::cout << n_test << std::endl;
    std::cout << random << std::endl;

    float total_schmidt = 0;
    float total_gauss = 0;
    double tValueSchmid = 0;
    double tValueGauss = 0;
    int n_diff = 0;
    
    for (int k=0; k<n_test; k++){
        // std::vector<Matrix<2>> foo;

        // if (random == 1){
        //     for (int j=0; j<s; j++){
        //         auto M = Matrix<2>(m, m, true);
        //         foo.push_back(M);
        //     }    
        // }
        // else{
            std::vector<std::vector<uInteger>> directionNumbersInit = {{}, {1},{1,3},{1,3,1}, 
                                                            {1,1,1},{1,1,3,3}, {1, 3, 5, 13}, {1, 1, 5, 5, 17}};
            std::vector<std::vector<uInteger>> directionNumbers;
            for (int i=0; i<s; i++){
                directionNumbers.push_back(directionNumbersInit[i]);
            }
            auto foo = SobolNet(m,s,directionNumbers);
        // }

        int maximalCardinality = s;
        auto compSchmid = ComputationScheme<tValueWeights,SchmidMethod,MaxFigure>(maximalCardinality,tValueWeights(s));
        compSchmid.extend(s-1);
        auto compGauss = ComputationScheme<tValueWeights,GaussMethod,MaxFigure>(maximalCardinality,tValueWeights(s));
        compGauss.extend(s-1);

        clock_t t1,t2, t3, t4;  
        bool verbose = false;   

        if (m <30){
            t1=clock();
            compSchmid.evaluateFigureOfMerit(foo,tValueSchmid);
            t2=clock();
            float diff ((float)t2-(float)t1);
            total_schmidt += diff;
        }
        
        t3=clock();
        compGauss.evaluateFigureOfMerit(foo,tValueGauss);
        t4=clock();
        float diff2 ((float)t4-(float)t3);
        total_gauss += diff2;

        if (m<30 && tValueGauss != tValueSchmid){
            n_diff++;
        }
    }
    std::cout << "n diff " << n_diff << std::endl;
    std::cout << "total schmid " << total_schmidt << std::endl;
    std::cout << "total gauss " << total_gauss << std::endl;
    

    return 0;
}
