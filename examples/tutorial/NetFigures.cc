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

#include <iostream>
#include <memory>

#include "netbuilder/Types.h"
#include "netbuilder/JoeKuo.h"
#include "netbuilder/FigureOfMerit/TValue.h"
#include "netbuilder/FigureOfMerit/CoordUniformFigureOfMerit.h"

#include "latticetester/ProductWeights.h"

#include "latbuilder/Kernel/PAlphaPLR.h"

using namespace NetBuilder;
using namespace NetBuilder::FigureOfMerit;
using JoeKuo::createJoeKuoSobolNet;

int main(int argc, char** argv)
{
    {
        std::cout << "Computing the t-value..." << std::endl;
        //! [figure1]
        auto figure = std::make_unique<TValue<EmbeddingType::UNILEVEL>>();
        //! [figure1]

        std::cout << figure->format() << std::endl;

        //! [evaluator1]
        auto evaluator = figure->evaluator();
        //! [evaluator1]

        //! [net1]
        auto net = createJoeKuoSobolNet(10, 15);
        //! [net1]

        //! [evaluation1]
        std::cout << "Merit value: " << (*evaluator)(net) << std::endl << std::endl;
        //! [evaluation1]
    }

    {
        std::cout << "Computing the P2..." << std::endl;
        //! [figure2]
        unsigned int alpha = 2;
        auto kernel = LatBuilder::Kernel::PAlphaPLR(alpha);
        auto weights = std::make_unique<LatticeTester::ProductWeights>(.7);
        auto figure = std::make_unique<CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR, EmbeddingType::UNILEVEL>>(std::move(weights), kernel);
        //! [figure2]

        std::cout << figure->format() << std::endl;

        //! [evaluator2]
        auto evaluator = figure->evaluator();
        //! [evaluator2]

        //! [net2]
        auto net = createJoeKuoSobolNet(10, 15);
        //! [net2]

        //! [evaluation2]
        std::cout << "Standard way:" << std::endl;
        std::cout << "Merit value: " << (*evaluator)(net) << std::endl;
        //! [evaluation2]

        //! [evaluation2-CBC]
        std::cout << "CBC-way:" << std::endl;
        MeritValue merit = 0; // start from a merit equal to zero
        for(Dimension coord = 0; coord < net.dimension(); ++coord) // for each coordinate
        {
            evaluator->prepareForNextDimension(); // prepare the evaluator for the next coordinate
            std::cout << "Begin coordinate: " << coord + 1 << "/" << net.dimension() << std::endl;
            merit = evaluator->operator()(net, coord, merit); // evaluate the partial merit value
            std::cout << "Partial merit value: " << merit << std::endl;
            evaluator->lastNetWasBest(); // tell the evaluator that the only net evaluated was best and shoud be kept for the next dimension
        }

        std::cout << "Merit value: " << (*evaluator)(net) << std::endl;
        //! [evaluation2-CBC]
    }
}