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

#include "netbuilder/DigitalNet.h"
#include "netbuilder/GeneratingMatrix.h"
#include "latbuilder/Util.h"

#include "Path.h"

using namespace NetBuilder;
using LatBuilder::PolynomialFromInt;

int main(int argc, char** argv)
{

    SET_PATH_TO_LATNETBUILDER_FOR_EXAMPLES();

    {
        //! [Sobol]
        std::cout << "Constructing Sobol' nets..." << std::endl;

        //! [Sobol_typedefs]
        typedef typename NetConstructionTraits<NetConstruction::SOBOL>::GenValue GeneratingValue;
        typedef typename NetConstructionTraits<NetConstruction::SOBOL>::SizeParameter SizeParameter;
        //! [Sobol_typedefs]

        //! [Sobol_first_net]
        GeneratingValue firstGenValue(0, {0});
        std::vector<GeneratingValue> genVals{firstGenValue};
        auto sobolNet = std::make_unique<DigitalNet<NetConstruction::SOBOL>>(1, SizeParameter(10), genVals);
        std::cout << "First generating matrix:" << std::endl << sobolNet->generatingMatrix(0) << std::endl;
        //! [Sobol_first_net]

        //! [Sobol_extend]
        GeneratingValue secondGenValue(1, {1});
        auto newSobolNet = sobolNet->extendDimension(secondGenValue);
        std::cout << "Second generating matrix:" << std::endl << newSobolNet->generatingMatrix(1) << std::endl;
        //! [Sobol_extend]

        //! [shared_memory]
        if (&(sobolNet->generatingMatrix(0)) == &(newSobolNet->generatingMatrix(0)))
            std::cout << "The shared generating matrix has a unique address." << std::endl;
        //! [shared_memory]

        std::cout << std::endl;
        //! [Sobol]
    }

    {
        //! [Polynomial]
        std::cout << "Constructing polynomial lattice rules..." << std::endl;

        //! [Polynomial_typedefs]
        typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::GenValue GeneratingValue;
        typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::SizeParameter Modulus;
        //! [Polynomial_typedefs]

        //! [Polynomial_modulus_and_first_net]
        Modulus modulus = PolynomialFromInt(1033);

        std::cout << "Modulus: " << modulus << std::endl;

        GeneratingValue firstGenValue = PolynomialFromInt(1);
        std::cout << "First generating value: " << firstGenValue << std::endl;

        std::vector<GeneratingValue> genVals{firstGenValue};

        auto polynomialNet = std::make_unique<DigitalNet<NetConstruction::POLYNOMIAL>>(1, modulus, genVals);
        std::cout << "First generating matrix:" << std::endl << polynomialNet->generatingMatrix(0) << std::endl;
        //! [Polynomial_modulus_and_first_net]

        //! [Polynomial_extend]
        GeneratingValue newGenValue = PolynomialFromInt(512);
        std::cout << "Second generating value: " << newGenValue << std::endl;

        auto newPolynomialNet = polynomialNet->extendDimension(newGenValue);
        std::cout << "Second generating matrix:" << std::endl << newPolynomialNet->generatingMatrix(1) << std::endl;
        //! [Polynomial_extend]

        std::cout << std::endl;
        //! [Polynomial]
    }

    {
        //! [Explicit]
        std::cout << "Constructing digital net with explicit generating matrices..." << std::endl;

        //! [Explicit_typedefs]
        typedef typename NetConstructionTraits<NetConstruction::EXPLICIT>::GenValue GeneratingValue;
        typedef typename NetConstructionTraits<NetConstruction::EXPLICIT>::SizeParameter SizeParameter;
        //! [Explicit_typedefs]

        //! [Explicit_net]
        GeneratingValue firstGenValue(10, 10, {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024});
        std::cout << "Second generating value: " << std::endl << firstGenValue << std::endl;     

        GeneratingValue newGenValue(10, 10, {1, 1, 0, 7, 0, 31, 0, 127, 0, 511, 0});
        std::cout << "Second generating value: " << std::endl << newGenValue << std::endl;  

        std::vector<GeneratingValue> genVals{firstGenValue, newGenValue};

        SizeParameter sizeParam(10,10);

        auto explicitNet = std::make_unique<DigitalNet<NetConstruction::EXPLICIT>>(2, sizeParam, genVals);
        //! [Explicit_net]
        
        //! [Explicit]
    }
}