// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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
#include "netbuilder/FigureOfMerit/CoordUniformFigureOfMerit.h"
#include "netbuilder/Task/ExhaustiveSearch.h"
#include "latticetester/ProductWeights.h"

#include "Path.h"

#include "latbuilder/Kernel/PAlphaTilde.h"

using namespace NetBuilder;
using namespace NetBuilder::FigureOfMerit;
using namespace NetBuilder::Task;

int main(int argc, char** argv)
{
        SET_PATH_TO_LATNETBUILDER_FOR_EXAMPLES();
        //! [search_params]
        unsigned int alpha = 2;
        auto kernel = LatBuilder::Kernel::PAlphaTilde(alpha);
        auto weights = std::make_unique<LatticeTester::ProductWeights>(.7);
        auto figure = std::make_unique<CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaTilde, EmbeddingType::UNILEVEL>>(std::move(weights), kernel);

        typedef typename NetConstructionTraits<NetConstruction::SOBOL>::SizeParameter SizeParameter;
        SizeParameter size(10);
        Dimension s = 5;
        //! [search_params]

        //! [task]
        auto task = std::make_unique<ExhaustiveSearch<NetConstruction::SOBOL, EmbeddingType::UNILEVEL>>(s, size, std::move(figure));
        std::cout << task->format();
        //! [task]

        //! [task_execute]
        task->execute();
        std::cout << "Best net:" << std::endl;
        std::cout << task->bestNet().format() << std::endl;
        std::cout << "Merit value: " << task->bestMeritValue() << std::endl;
        //! [task_execute]
}