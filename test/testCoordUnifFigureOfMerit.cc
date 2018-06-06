// // This file is part of LatNet Builder.
// //
// // Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
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

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/CoordUniformFigureOfMerit.h"

#include "latbuilder/Kernel/PAlphaPLR.h"
#include "latbuilder/Types.h"
#include "latbuilder/SizeParam.h"

#include "netbuilder/Task/Eval.h"

#include "latticetester/Weights.h"
#include "latticetester/UniformWeights.h"

#include <iostream>

using namespace NetBuilder;

unsigned int getmsb (unsigned long long x){
    unsigned int r = 0;
    if (x < 1) return 0;
    while (x >>= 1) r++;
    return r;
}

Real combine(const RealVector& vec){
        Real sum_of_elems = 0;
        for (auto& n : vec){
            std::cout << n << " ";
            sum_of_elems += n;
        }
        std::cout << std::endl;
        return sum_of_elems;
    }

int main(int argc, const char *argv[])
{
    unsigned int s = 5;
    unsigned int m = 5;

    // auto matrices = std::vector<std::shared_ptr<GeneratingMatrix>>(s);
    // matrices[1] = std::make_shared<GeneratingMatrix>(m, m, std::vector<uInteger>({8, 12, 14, 7}));
    // matrices[0] = std::make_shared<GeneratingMatrix>(m, m, std::vector<uInteger>({10, 5, 2, 9}));
    // matrices[2] = std::make_shared<GeneratingMatrix>(m, m, std::vector<uInteger>({6, 11, 5, 2}));
    // matrices[3] = std::make_shared<GeneratingMatrix>(m, m, std::vector<uInteger>({15, 7, 11, 5}));
    // auto net = std::make_unique<DigitalNet>(s, m, m, matrices);

    // auto net = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(s, m, m);

    auto net = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(s, m);

    std::cout << "Constructing..." << std::endl;

    unsigned int modulus = intPow(2, m);

    auto weights1 = std::make_unique<LatticeTester::ProductWeights>(1.);

    auto kernel = LatBuilder::Kernel::PAlphaPLR(2);

    LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, LatBuilder::PointSetType::UNILEVEL> param(intPow(2, m));
    auto fig1 = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR, LatBuilder::PointSetType::UNILEVEL>>(std::move(weights1), param, kernel);

    // LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, LatBuilder::PointSetType::MULTILEVEL> param(2, m);
    // std::function<Real (const RealVector&)> f_combine = combine;
    // auto fig1 = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR, LatBuilder::PointSetType::MULTILEVEL>>(std::move(weights1), m, param, kernel, f_combine);
    
    auto eval = fig1->evaluator();

    std::cout << "Executing..." << std::endl;

    // std::cout << (*eval)(*net) << std::endl;

    auto task = Task::Eval(std::move(net),std::move(fig1));

    task.execute();

    std::cout << task.meritValue() << std::endl;

    return 0;
}
