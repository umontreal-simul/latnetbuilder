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
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/ProgressiveRowReducer.h"


#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/ResolutionGapProjMerit.h"
#include "netbuilder/FigureOfMerit/UniformityProperties.h"

#include "latticetester/Weights.h"
#include "latticetester/UniformWeights.h"
#include "latticetester/CoordinateSets.h"

#include "latbuilder/LFSR258.h"

#include <iostream>
#include <algorithm>
#include <boost/numeric/ublas/blas.hpp>

using namespace NetBuilder;

int main(int argc, const char *argv[])
{
    unsigned int s = 50;
    unsigned int m = 30;

    auto net = DigitalNetConstruction<NetConstruction::SOBOL>(s,m,m);


    auto weights = std::make_unique<LatticeTester::UniformWeights>(1);

    // auto projDep = std::make_unique<FigureOfMerit::ResolutionGapProjMerit<NetEmbed::SIMPLE>>(s);

    // auto fig = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::ResolutionGapProjMerit<NetEmbed::SIMPLE>>>(1, std::move(weights), std::move(projDep), OpMax());
    
    auto projDep = std::make_unique<FigureOfMerit::ResolutionGapProjMerit<NetEmbed::EMBEDDED>>(2, Combiner(boost::numeric::ublas::blas_1::amax<RealVector>));

    auto fig = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::ResolutionGapProjMerit<NetEmbed::EMBEDDED>>>(1, std::move(weights), std::move(projDep), OpMax());

    auto eval = fig->evaluator();

    (*eval)(net,true);
    
    return 0;
}
