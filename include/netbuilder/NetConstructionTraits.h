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

#ifndef NETBUILDER__NET_CONSTRUCTION_TRAITS_H
#define NETBUILDER__NET_CONSTRUCTION_TRAITS_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/GeneratingMatrix.h"

#include <memory>

namespace NetBuilder {

/** Digital net construction traits.
 *  Specialization of this class must define the type:
 *  - GenValue: type of the generating value required to construct a generating matrix
 * \n the following static functions:
 *  - static bool checkGenValue(const GenValue& genValue): checks whether a generating value is correct
 *  - static GeneratingMatrix createGeneratingMatrix(const GenValue& genValue, unsigned int nRows, unsigned int nCols): 
 * create a generating matrix of the given shape from the generating value
 *  - static std::vector<GenValue> defaultGenValues(unsigned int dimension): returns a vector of default generating values
 *  (one for each coordinates lower than dimension)
 *  - static std::vector<GenValue> genValueSpace(unsigned int dimension): returns a vector of all the possible generating values
 *  for the given dimension
 * \n and the following class template:
 *  - template<typename RAND> class RandomGenValueGenerator: a class template where template parameter RAND implements
 *  a C++11 type PRNG. This is a random generator of generating values. This class template must define a constructor 
 *  RandomGenValueGenerator(RAND randomGen = RAND()) and an the member function GenValue operator()(unsigned int dimenion) returning
 *  a generating value for the given dimension.
 */ 
    template <NetConstruction NC>
    struct NetConstructionTraits;



    template<>
    struct NetConstructionTraits<NetConstruction::SOBOL>
    {
        public:
            typedef std::pair<unsigned int, std::vector<uInteger>> GenValue ;

            static bool checkGenValue(const GenValue& genValue);

            static GeneratingMatrix* createGeneratingMatrix(const GenValue& genValue, unsigned int nRows, unsigned int nCols);

            /*
            static void extendGeneratingMatrices( 
                unsigned int inc,
                const std::vector<std::shared_ptr<GeneratingMatrix>>& genMats, 
                const std::vector<std::shared_ptr<GenValue>>& genValues);
            */    

            static std::vector<GenValue> defaultGenValues(unsigned int dimension);

            static std::vector<GenValue> genValueSpace(unsigned int dimension);

            template<typename RAND>
            class RandomGenValueGenerator
            {
                public:
                    RandomGenValueGenerator(RAND randomGen = RAND()):
                        m_randomGen(std::move(randomGen))
                    {};
                    
                    GenValue operator()(unsigned int dimension)
                    {
                        unsigned int size;
                        if (dimension==1)
                        {
                            size = 1;
                        }
                        else
                        {
                            size = nthPrimitivePolynomialDegree(dimension-1);
                        }
                        std::vector<unsigned long> res(size);
                        for(unsigned int k = 0; k < size; ++k)
                        {
                            res[k] = 2 * (m_randomGen() % (1 << k)) + 1 ; 
                        }
                        return GenValue(dimension,std::move(res));
                    }

                private:
                    RAND m_randomGen;
            };

        private:
            typedef std::pair<unsigned int,uInteger> PrimitivePolynomial; 

            static PrimitivePolynomial nthPrimitivePolynomial(unsigned int n);

            static unsigned int  nthPrimitivePolynomialDegree(unsigned int n);

            static std::vector<std::vector<uInteger>> readJoeKuoDirectionNumbers(unsigned int dimension);


    };
   

//    template<>
//     struct NetConstructionTraits<NetConstruction::UNIRANDOM>
//     {
//         public:
//             typedef unsigned int GenValue ;

//             static bool checkGenValue(const GenValue& genValue);

//             static GeneratingMatrix* createGeneratingMatrix(const GenValue& genValue, unsigned int nRows, unsigned int nCols);

//             static void extendGeneratingMatrices( 
//                 unsigned int inc,
//                 const std::vector<std::shared_ptr<GeneratingMatrix>>& genMats, 
//                 const std::vector<std::shared_ptr<GenValue>>& genValues);
            
//             static std::vector<GenValue> defaultGenValues(unsigned int dimension);
//     };

}

#endif