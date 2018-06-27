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

/**
 * \file 
 * This file contains the various methods used to construct generating matrices of digital nets
 */
 
#ifndef NETBUILDER__NET_CONSTRUCTION_TRAITS_H
#define NETBUILDER__NET_CONSTRUCTION_TRAITS_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/GeneratingMatrix.h"

#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/SeqCombiner.h"
#include "latbuilder/UniformUIntDistribution.h"

#include <string>
#include <list>
#include <tuple>
#include <string>
#include <vector>
#include <utility>

#include <boost/dynamic_bitset.hpp>



namespace NetBuilder {

/** Digital net construction traits.
 *  Specialization of this class must define:
 * \n the following types:
 *  - \c GenValue: type of the value required to construct a generating matrix (specific to a coordinate)
 *  - \c DesignParameter: type of the parameter used to construct a net (shared by all coordinates)
 * \n the following static variables:
 *  - \c defaultDesignParameter: a default design parameter
 *  - \c isSequenceViewable: a bool indicating whether the net can be viewed as a sequence
 * \n the following static functions:
 *  - <CODE> static \c bool \c checkGenValue(const GenValue& genValue) </CODE>: checks whether a generating value is correct
 *  - <CODE> static \c unsigned int \c nRows(const GenValue& genValue) </CODE>: computes the number of rows associated to the design parameter
 *  - <CODE> static \c unsigned int \c nCols(const GenValue& genValue) </CODE>: computes the number of columns associated to the design parameter
 *  - <CODE>static GeneratingMatrix* createGeneratingMatrix(const GenValue& genValue, DesignParameter designParameter)</CODE>: 
 * create a generating matrix using the generating value and the design parameter.
 *  - <CODE>static std::vector<GenValue> defaultGenValues(Dimension dimension, const DesignParameter& designParameter))</CODE>: returns a vector of default generating values
 *  (one for each coordinates lower than dimension)
 *  - <CODE>static std::vector<GenValue> genValueSpaceCoord(Dimension coord, const DesignParameter& designParameter)</CODE>: returns a vector of all the possible generating values
 *  for the given dimension
 *  - <CODE> static std::vector<std::vector<GenValue>> genValueSpace(Dimension dimension , const DesignParameter& designParameter) </CODE>: returns a vector of all the possible combination 
 *  of the generating values for dimensions lower than \c dimension
 * \n and the following class template:
 *  - <CODE> template<typename RAND> class RandomGenValueGenerator </CODE>: a class template where template parameter RAND implements
 *  a C++11 type PRNG. This is a random generator of generating values. This class template must define a constructor 
 *  <CODE> RandomGenValueGenerator(DesignParameter designParameter, RAND randomGen = RAND()) </CODE> and an the member function <CODE>GenValue operator()(unsigned int dimenion)</CODE> returning
 *  a generating value for \c dimension.
 */ 
template <NetConstruction NC>
struct NetConstructionTraits;

template<>
struct NetConstructionTraits<NetConstruction::SOBOL>
{
    public:
        typedef std::pair<Dimension, std::vector<uInteger>> GenValue ;

        typedef unsigned int DesignParameter;

        static DesignParameter defaultDesignParameter;

        static constexpr bool isSequenceViewable = true;

        static constexpr bool hasSpecialFirstCoordinate = true;

        static bool checkGenValue(const GenValue& genValue, const DesignParameter& designParam);

        static unsigned int nRows(const DesignParameter& param);

        static unsigned int nCols(const DesignParameter& param);

        static GeneratingMatrix* createGeneratingMatrix(const GenValue& genValue, const DesignParameter& designParam);

        class GenValueSpaceCoordSeq
        {
            public:

                typedef GenValue value_type;
                typedef size_t size_type;

                GenValueSpaceCoordSeq(Dimension coord);

                class const_iterator:
                public boost::iterators::iterator_facade<const_iterator,
                const GenValue,
                boost::iterators::forward_traversal_tag>
                {
                    public:
                        struct end_tag {};

                        explicit const_iterator(const GenValueSpaceCoordSeq& seq);

                        const_iterator(const GenValueSpaceCoordSeq& seq, end_tag);

                    private:
                        friend class boost::iterators::iterator_core_access;

                        bool equal(const const_iterator& other) const;

                        const value_type& dereference() const;

                        void increment();

                        Dimension m_coord;

                        LatBuilder::SeqCombiner<std::vector<uInteger>,LatBuilder::CartesianProduct>::const_iterator m_underlyingIterator;

                        value_type m_value;

                };

                const_iterator begin() const;;

                const_iterator end() const;

                Dimension coord() const;

                size_t size() const;

                const LatBuilder::SeqCombiner<std::vector<uInteger>,LatBuilder::CartesianProduct>& underlyingSeq() const;

            private:
                Dimension m_coord;
                LatBuilder::SeqCombiner<std::vector<uInteger>,LatBuilder::CartesianProduct> m_underlyingSeq;

                static std::vector<std::vector<uInteger>> underlyingSeqs(Dimension coord);
        };

        typedef LatBuilder::SeqCombiner<GenValueSpaceCoordSeq, LatBuilder::CartesianProduct> GenValueSpaceSeq;
        
        static std::vector<GenValue> defaultGenValues(Dimension dimension, const DesignParameter& designParameter);

        static GenValueSpaceCoordSeq genValueSpaceCoord(Dimension coord, const DesignParameter& designParameter);

        static GenValueSpaceSeq genValueSpace(Dimension dimension , const DesignParameter& designParameter);

        template<typename RAND>
        class RandomGenValueGenerator
        {
            public:
                RandomGenValueGenerator(DesignParameter designParameter, RAND randomGen = RAND()):
                    m_designParameter(std::move(designParameter)),
                    m_randomGen(std::move(randomGen)),
                    m_unif(0, 1)
                {};
                
                GenValue operator()(Dimension coord)
                {
                    unsigned int size;
                    if (coord==0)
                    {
                        return GenValue(0, {0});
                    }
                    else
                    {
                        size = nthPrimitivePolynomialDegree(coord);
                    }
                    std::vector<unsigned long> res(size);
                    unsigned long upperBound = 0;
                    for(unsigned int k = 0; k < size; ++k)
                    {
                        m_unif.setUpperBound(upperBound);
                        res[k] = 2 * m_unif(m_randomGen) + 1 ;
                        upperBound = 2 * (upperBound + 1) - 1; 
                    }
                    return GenValue(coord,std::move(res));
                }

            private:
                DesignParameter m_designParameter;
                RAND m_randomGen;
                LatBuilder::UniformUIntDistribution<unsigned long, RAND> m_unif;
        };

        static std::string format(const std::vector<std::shared_ptr<GenValue>>& genVals, const DesignParameter& designParameter, OutputFormat outputFormat);

    private:
        typedef std::pair<unsigned int,uInteger> PrimitivePolynomial; 

        static PrimitivePolynomial nthPrimitivePolynomial(Dimension n);

        static unsigned int  nthPrimitivePolynomialDegree(Dimension n);

        static std::vector<std::vector<uInteger>> readJoeKuoDirectionNumbers(Dimension dimension);
};

template<>
struct NetConstructionTraits<NetConstruction::POLYNOMIAL>
{
    public:
        typedef Polynomial GenValue ;

        typedef Polynomial DesignParameter;

        static DesignParameter defaultDesignParameter;

        typedef LatBuilder::GenSeq::GeneratingValues<LatBuilder::LatticeType::POLYNOMIAL, LatBuilder::Compress::NONE> GenValueSpaceCoordSeq;

        typedef LatBuilder::SeqCombiner<GenValueSpaceCoordSeq, LatBuilder::CartesianProduct> GenValueSpaceSeq;

        static constexpr bool isSequenceViewable = false;

        static constexpr bool hasSpecialFirstCoordinate = true;

        static bool checkGenValue(const GenValue& genValue, const DesignParameter& designParam);

        static unsigned int nRows(const DesignParameter& param);

        static unsigned int nCols(const DesignParameter& param);

        static GeneratingMatrix* createGeneratingMatrix(const GenValue& genValue, const DesignParameter& designParam);
    
        static std::vector<GenValue> defaultGenValues(Dimension dimension, const DesignParameter& designParameter);

        static GenValueSpaceCoordSeq genValueSpaceCoord(Dimension coord, const DesignParameter& designParameter);

        static GenValueSpaceSeq genValueSpace(Dimension dimension , const DesignParameter& designParameter);

        template<typename RAND>
        class RandomGenValueGenerator
        {
            public:
                RandomGenValueGenerator(DesignParameter designParameter, RAND randomGen = RAND()):
                    m_randomGen(std::move(randomGen)),
                    m_generatingValues(LatBuilder::GenSeq::GeneratingValues<LatBuilder::LatticeType::POLYNOMIAL, LatBuilder::Compress::NONE>(std::move(designParameter))),
                    m_totient(m_generatingValues.size()),
                    m_unif(0, m_totient - 1)
                {}
                
                GenValue operator()(Dimension dimension)
                {
                    if (dimension==0)
                    {
                        return GenValue(1);
                    }
                    else
                    {
                        return m_generatingValues[m_unif(m_randomGen)];
                    }
                }
            private:
                RAND m_randomGen;
                LatBuilder::GenSeq::GeneratingValues<LatBuilder::LatticeType::POLYNOMIAL, LatBuilder::Compress::NONE> m_generatingValues;
                size_t m_totient;
                LatBuilder::UniformUIntDistribution<size_t, RAND> m_unif;
        };

        static std::string format(const std::vector<std::shared_ptr<GenValue>>& genVals, const DesignParameter& designParameter, OutputFormat outputFormat);
};

template<>
struct NetConstructionTraits<NetConstruction::EXPLICIT>
{
    public:
        typedef GeneratingMatrix GenValue ;

        typedef std::pair<unsigned int, unsigned int> DesignParameter;

        static DesignParameter defaultDesignParameter;

        typedef std::vector<GenValue> GenValueSpaceCoordSeq;

        static constexpr bool isSequenceViewable = false;

        static constexpr bool hasSpecialFirstCoordinate = false;

        static bool checkGenValue(const GenValue& genValue, const DesignParameter& designParam);

        static unsigned int nRows(const DesignParameter& param);

        static unsigned int nCols(const DesignParameter& param);

        static GeneratingMatrix* createGeneratingMatrix(const GenValue& genValue, const DesignParameter& designParam);
    
        static std::vector<GenValue> defaultGenValues(Dimension dimension, const DesignParameter& designParameter);

        static GenValueSpaceCoordSeq genValueSpaceCoord(Dimension coord, const DesignParameter& designParameter);

        static std::vector<GenValueSpaceCoordSeq> genValueSpace(Dimension dimension , const DesignParameter& designParameter);

        template<typename RAND>
        class RandomGenValueGenerator
        {
            public:
                RandomGenValueGenerator(DesignParameter designParameter, RAND randomGen = RAND()):
                    m_designParameter(std::move(designParameter)),
                    m_randomGen(std::move(randomGen)),
                    m_unif(0, (1 << m_designParameter.second) - 1)
                {};
                
                GenValue operator()(Dimension dimension)
                {
                    std::vector<uInteger> init;
                    init.reserve(m_designParameter.first);
                    for(unsigned int i = 0; i < m_designParameter.second; ++i)
                    {
                        init.push_back(m_unif(m_randomGen));
                    }
                    return GeneratingMatrix(m_designParameter.first, m_designParameter.second, std::move(init));
                }
            private:
                DesignParameter m_designParameter;
                RAND m_randomGen;
                LatBuilder::UniformUIntDistribution<unsigned long, RAND> m_unif;
                std::vector<GenValue> m_primes;
                uInteger m_totient;
        };

        static std::string format(const std::vector<std::shared_ptr<GenValue>>& genVals, const DesignParameter& designParameter, OutputFormat outputFormat);
};

}

#endif