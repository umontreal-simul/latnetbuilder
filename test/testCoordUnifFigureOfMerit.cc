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

// // #include <NTL/GF2X.h>
// // #include <NTL/GF2XFactoring.h>

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/GeneratingMatrix.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/CoordUniformFigureOfMerit.h"

#include "latbuilder/Kernel/PAlphaPLR.h"
#include "latbuilder/Types.h"
#include "latbuilder/detail/TextStream.h"

#include "netbuilder/Task/Eval.h"

#include "latcommon/Weights.h"
#include "latcommon/UniformWeights.h"
#include "latcommon/CoordinateSets.h"

#include <iostream>

using namespace NetBuilder;
// using namespace LatBuilder::TextStream;
// using namespace LatBuilder;

unsigned int
      getmsb (unsigned long long x)
      {
          unsigned int r = 0;
          if (x < 1) return 0;
          while (x >>= 1) r++;
          return r;
      }

int main(int argc, const char *argv[])
{
    unsigned int s = 2;
    unsigned int m = 4;

    // GeneratingMatrix m2 {m, m, std::vector<uInteger>({16, 8, 20, 10, 5})};
    // std::cout << m2 << std::endl;

    // auto matrices = std::vector<std::shared_ptr<GeneratingMatrix>>(s);
    // matrices[0] = std::make_shared<GeneratingMatrix>(m, m, std::vector<uInteger>({8, 12, 14, 7}));
    // matrices[1] = std::make_shared<GeneratingMatrix>(m, m, std::vector<uInteger>({6, 11, 5, 2}));
    // matrices[2] = std::make_shared<GeneratingMatrix>(m, m, std::vector<uInteger>({6, 11, 5, 2}));
    // matrices[3] = std::make_shared<GeneratingMatrix>(m, m, std::vector<uInteger>({15, 7, 11, 5}));
    // // matrices[4] = std::make_shared<GeneratingMatrix>(m, m, std::vector<uInteger>({1, 0, 0, 0}));
    // std::cout << *(matrices[0]) << std::endl;
    // std::cout << *(matrices[1]) << std::endl;

    // auto net = std::make_unique<DigitalNet>(s, m, m, matrices);
    auto net = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(s, m, m);

    // auto stride = net->generatingMatrix(1);
    // std::cout << stride << std::endl;
    // std::vector<unsigned int> m_permutation (1 << stride.nCols(), 0);
    //   {
    //     for (unsigned int i=1; i<m_permutation.size(); ++i){
    //       m_permutation[i] = m_permutation[i-1] ^ stride.getCol(getmsb(((i >> 1) ^ i)^(((i-1) >> 1) ^ (i-1))));
    //       std::cout << m_permutation[i] << std::endl;
    //     }
    //   }

    
    unsigned int modulus = intPow(2, m);

    LatBuilder::Storage<LatBuilder::LatticeType::DIGITAL, LatBuilder::LatEmbed::SIMPLE,  LatBuilder::Compress::NONE> m_storage = LatBuilder::SizeParam<LatBuilder::LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE> {(typename LatBuilder::LatticeTraits<LatBuilder::LatticeType::ORDINARY>::Modulus)(modulus)};            
    std::cout << m_storage.size() << std::endl;
    std::cout << m_storage.virtualSize() << std::endl;

    // RealVector vec(m_storage.size());
    // auto proxy = m_storage.unpermuted(vec);

    // for (size_t i = 0; i < vec.size(); i++)
    //     proxy(i) = Real(LatBuilder::LatticeTraits<LatBuilder::LatticeType::DIGITAL>::ToKernelIndex(i,modulus)) / modulus;

    // std::cout << proxy.size() << std::endl;
    // for (size_t i = 0; i < vec.size(); i++){
    //     // std::cout << proxy(i) << std::endl;
    //     std::cout << vec[i] << std::endl;
    // }
 

    auto weights1 = std::make_unique<LatCommon::OrderDependentWeights>(0.1);

    auto kernel = LatBuilder::Kernel::PAlphaPLR(2);

    auto fig1 = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR>>(std::move(weights1), m, kernel);
    // auto eval = fig1->evaluator();
    // auto kernel_values = eval->m_innerProd.kernelValues();

    // for (int i=0; i<(kernel_values).size(); i++){
    //     std::cout << kernel_values[i] << " ";
    // }
    std::cout << std::endl;

    // // std::cout << *fig1 << std::endl;

    // (*eval)(*net, 1, 0);

    // for (int i=0; i<(eval->weightedState()).size(); i++){
    //     std::cout << eval->weightedState()[i] << " ";
    // }
    // std::cout << std::endl;

    // (*eval)(*net, 2, 0);

    // for (int i=0; i<(eval->weightedState()).size(); i++){
    // std::cout << eval->weightedState()[i] << " ";
    // }
    // std::cout << std::endl;

    // std::cout << eval->weightedState() << std::endl;

    auto task = Task::Eval(std::move(net),std::move(fig1));

    task.execute();

    std::cout << task.meritValue() << std::endl;

    return 0;
}

// // int main(){
// //     return 0;
// // }

// #include <NTL/ZZ.h>
// #include <NTL/vector.h>

// int main(int argc, const char * argv[]) {

// NTL::Vec<NTL::ZZ> v;
// v.SetLength(8);
// return 0;
// }
