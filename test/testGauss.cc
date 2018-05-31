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

#include <map>
#include <fstream>
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/ProgressiveRowReducer.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/TValueComputation.h"
#include <time.h>
#include <chrono>

#include "latbuilder/LFSR258.h"

using namespace NetBuilder;

int main(int argc, const char *argv[])
{        
    int m = 20;
    int s = 10;
    int nLevel=m;

    typedef NetConstructionTraits<NetConstruction::SOBOL> ConstructionMethod;

    typedef ConstructionMethod:: template RandomGenValueGenerator <LatBuilder::LFSR258> randomGenValueGenerator;
    randomGenValueGenerator randomGen = randomGenValueGenerator();

    auto net = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(1, m);
    for (int dim=2; dim<=s; dim++){
        net = net->extendDimension(randomGen(dim));
    }

    // auto net = DigitalNetConstruction<NetConstruction::SOBOL>(s,m);
    

    std::vector<std::vector<GeneratingMatrix>> v;
    for (int level=0; level<nLevel; level++){
        std::vector<GeneratingMatrix> gen;
        for (int i=1; i<=s; i++){
            gen.push_back(net->pointerToGeneratingMatrix(i)->subMatrix(m, m-level));
        }
        v.push_back(gen);
    }
    

    clock_t t1,t2, t3, t4;
    float diff1, diff2 = 0;

    std::vector<int> iNew;
    std::vector<int> iNew2;
    std::vector<unsigned int> iOld; 
    std::vector<unsigned int> maxSubProj (nLevel, 0);
    for (int i=0; i<10; i++){
        t1=clock();
        for (int level=nLevel-1; level>=0; level--){
            iNew.push_back(SchmidMethod::computeTValue(v[level], 0, 0));
        }
        t2=clock();
        t3=clock();
        iOld = GaussMethod::computeTValue(v[0], m-nLevel, maxSubProj, 0);
        t4=clock();
        diff1 += ((float)t2-(float)t1);
        diff2 += ((float)t4-(float)t3);
    }
    std::cout << "Schmid method s=" << s << ", m=" << m << ", time= " << diff1 <<std::endl;
    for (int level=0; level<nLevel; level++){
        std::cout << iNew[level] << " ";
    }
    std::cout << std::endl;

    std::cout << "Gauss method  s=" << s << ", m=" << m << ", time= " << diff2 <<std::endl;
    for (int level=0; level<nLevel; level++){
        std::cout << iOld[level] << " ";
    }
    std::cout  << std::endl;    

    return 0;
}
