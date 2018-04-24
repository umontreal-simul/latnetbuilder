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

#ifndef NET_BUILDER__NET_CONSTRUCTION_TRAITS_H
#define NET_BUILDER__NET_CONSTRUCTION_TRAITS_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/GeneratingMatrix.h"

#include <memory>

namespace NetBuilder {

    template <NetConstruction NC>
    struct NetConstructionTraits;

    template<>
    struct NetConstructionTraits<NetConstruction::SOBOL>
    {
        public:
            typedef std::pair<unsigned int, std::vector<uInteger>> GenValue ;

            static bool checkGenValue(const GenValue& genValue);

            static GeneratingMatrix* createGeneratingMatrix(const GenValue& genValue, unsigned int nRows, unsigned int nCols);

            static void extendGeneratingMatrices( 
                unsigned int inc,
                const std::vector<std::shared_ptr<GeneratingMatrix>>& genMats, 
                const std::vector<std::shared_ptr<GenValue>>& genValues);

            static std::vector<GenValue> defaultGenValues(unsigned int dimension);

        private:
            typedef std::pair<unsigned int,uInteger> PrimitivePolynomial; 

            static PrimitivePolynomial nthPrimitivePolynomial(unsigned int n);

            static unsigned int  nthPrimitivePolynomialDegree(unsigned int n);

            static std::vector<std::vector<uInteger>> readJoeKuoDirectionNumbers(unsigned int dimension);


    };

/*    template<>
    struct NetConstructionTraits<NetConstruction::EXPLICIT>
    {
        public:
            typedef GeneratingMatrix GenValue ;

            static bool checkGenValue(const GenValue& genValue);

            static GeneratingMatrix* createGeneratingMatrix(const GenValue& genValue);

            static std::vector<std::shared_ptr<GeneratingMatrices>> extendGeneratingMatrices( 
                const std::vector<std::shared_ptr<GeneratingMatrices>>& genMats, 
                const std::vector<std::shared_ptr<GenValue>>& genValues);
    };*/


    template<>
    struct NetConstructionTraits<NetConstruction::UNIRANDOM>
    {
        public:
            typedef unsigned int GenValue ;

            static bool checkGenValue(const GenValue& genValue);

            static GeneratingMatrix* createGeneratingMatrix(const GenValue& genValue, unsigned int nRows, unsigned int nCols);

            static void extendGeneratingMatrices( 
                unsigned int inc,
                const std::vector<std::shared_ptr<GeneratingMatrix>>& genMats, 
                const std::vector<std::shared_ptr<GenValue>>& genValues);
            
            static std::vector<GenValue> defaultGenValues(unsigned int dimension);
    };
}

#endif