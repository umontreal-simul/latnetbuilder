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
#include "netbuilder/DigitalNet.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"
#include "netbuilder/Task/CBCSearch.h"
#include "netbuilder/Task/FullCBCExplorer.h"
#include "netbuilder/Task/RandomCBCExplorer.h"
#include "netbuilder/Task/MixedCBCExplorer.h"
#include "latticetester/ProductWeights.h"

#include "Path.h"

#include "latbuilder/Util.h"

using namespace NetBuilder;
using namespace NetBuilder::FigureOfMerit;
using namespace NetBuilder::Task;
using LatBuilder::PolynomialFromInt;
int main(int argc, char** argv)
{
        SET_PATH_TO_LATNETBUILDER_FOR_EXAMPLES();
        {
                //! [search_params]
                auto weights = std::make_unique<LatticeTester::ProductWeights>(.7);
                auto projDepMerit = std::make_unique<TValueProjMerit<EmbeddingType::UNILEVEL>>(3);
                auto figure = std::make_unique<WeightedFigureOfMerit<TValueProjMerit<EmbeddingType::UNILEVEL>>>(std::numeric_limits<Real>::infinity(), std::move(weights), std::move(projDepMerit));
                typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::SizeParameter SizeParameter;
                SizeParameter size = PolynomialFromInt(1033);
                Dimension s = 5;
                //! [search_params]

                //! [full-CBC_explorer]
                auto explorer = std::make_unique<FullCBCExplorer<NetConstruction::POLYNOMIAL, EmbeddingType::UNILEVEL>>(s, size);
                auto task = std::make_unique<CBCSearch<NetConstruction::POLYNOMIAL, EmbeddingType::UNILEVEL, FullCBCExplorer>>(s, size, std::move(figure), std::move(explorer));
                //! [full-CBC_explorer]

                //! [execute_task]
                std::cout << task->format();
                task->execute();
                std::cout << "Best net: " << task->bestNet().format() << std::endl;
                std::cout << "Merit value: " << task->bestMeritValue() << std::endl;
                //! [execute_task]
        }

        {
                auto weights = std::make_unique<LatticeTester::ProductWeights>(.7);
                auto projDepMerit = std::make_unique<TValueProjMerit<EmbeddingType::UNILEVEL>>(3);
                auto figure = std::make_unique<WeightedFigureOfMerit<TValueProjMerit<EmbeddingType::UNILEVEL>>>(std::numeric_limits<Real>::infinity(), std::move(weights), std::move(projDepMerit));
                typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::SizeParameter SizeParameter;
                SizeParameter size = PolynomialFromInt(1033);
                Dimension s = 5;

                //! [random-CBC_explorer]
                auto explorer = std::make_unique<RandomCBCExplorer<NetConstruction::POLYNOMIAL, EmbeddingType::UNILEVEL>>(s, size, 70);
                auto task = std::make_unique<CBCSearch<NetConstruction::POLYNOMIAL, EmbeddingType::UNILEVEL, RandomCBCExplorer>>(s, size, std::move(figure), std::move(explorer));
                //! [random-CBC_explorer]

                std::cout << task->format();
                task->execute();
                std::cout << "Best net: " << task->bestNet().format() << std::endl;
                std::cout << "Merit value: " << task->bestMeritValue() << std::endl;
        }

        {
                auto weights = std::make_unique<LatticeTester::ProductWeights>(.7);
                auto projDepMerit = std::make_unique<TValueProjMerit<EmbeddingType::UNILEVEL>>(3);
                auto figure = std::make_unique<WeightedFigureOfMerit<TValueProjMerit<EmbeddingType::UNILEVEL>>>(std::numeric_limits<Real>::infinity(), std::move(weights), std::move(projDepMerit));
                typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::SizeParameter SizeParameter;
                SizeParameter size = PolynomialFromInt(1033);
                Dimension s = 5;

                //! [mixed-CBC_explorer]
                auto explorer = std::make_unique<MixedCBCExplorer<NetConstruction::POLYNOMIAL, EmbeddingType::UNILEVEL>>(s, size, 3, 70);
                auto task = std::make_unique<CBCSearch<NetConstruction::POLYNOMIAL, EmbeddingType::UNILEVEL, MixedCBCExplorer>>(s, size, std::move(figure), std::move(explorer));
                //! [mixed-CBC_explorer]

                std::cout << task->format();
                task->execute();
                std::cout << "Best net: " << task->bestNet().format() << std::endl;
                std::cout << "Merit value: " << task->bestMeritValue() << std::endl;
        }

}