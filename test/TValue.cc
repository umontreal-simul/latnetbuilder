// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
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

#include "netbuilder/FigureOfMerit/TValue.h"

using namespace NetBuilder;

int main(int argc, char** argv)
{

    Dimension s = std::atol(argv[1]);
    unsigned int k = std::atoi(argv[2]);

    auto figure = std::make_unique<FigureOfMerit::TValue<EmbeddingType::UNILEVEL>>();
    auto eval = figure->evaluator();
    auto net = JoeKuo::createJoeKuoSobolNet(s, k);

    std::cout << "t-value: " << (*eval)(net) << std::endl;

    auto weights = std::make_unique<LatticeTester::OrderDependentWeights>();
    weights->setWeightForOrder(s, 1);

    auto projDep = std::make_unique<FigureOfMerit::TValueProjMerit<EmbeddingType::UNILEVEL>>(s);

    auto fig = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<EmbeddingType::UNILEVEL>>>(std::numeric_limits<Real>::infinity(), std::move(weights), std::move(projDep));

    auto evalo = fig->evaluator();

    std::cout << "t-value: " << (*evalo)(net) << std::endl;
}



#endif
