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

#ifndef NET_BUILDER__FIGURE_OF_MERIT_BIT__UNIFORMITY_PROPERTY
#define NET_BUILDER__FIGURE_OF_MERIT_BIT__UNIFORMITY_PROPERTY

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/ProgressiveRowReducer.h"


namespace NetBuilder { namespace FigureOfMerit {

/** 
 * Class which represents uniformity properties for digital sequences. Uniformity properties generalize the so-called Property A and Property A'
 * introduced by Sobol.
 * Ref: TODO
 *  Sobol, I. M. (1976) "Uniformly distributed sequences with an additional uniform property". Zh. Vych. Mat. Mat. Fiz. 16: 1332–1337 (in Russian); U.S.S.R. Comput. Maths. Math. Phys. 16: 236–242 (in English).
 * A digital sequence in base 2 in dimension \f$ s \f$ is said to have the \f$ k \f$ bits uniformity property if its \f$ 2^s \f$ first points are
 * \f$ k \f$ bit-equidistributed.
 */ 

class UniformityProperty : public FigureOfMerit{

    public:

        /** 
         * Constructor.
         * @param nbBits Number of equidistribution bits.
         * @param weight Weight of the figure of merit. Default to +inf.
         * @param normType Norm type of the figure. Default to sup norm.
         */
        UniformityProperty(unsigned int nbBits, Real weight = std::numeric_limits<Real>::infinity(), Real normType = std::numeric_limits<Real>::infinity()):
            m_nbBits(nbBits),
            m_weight(weight),
            m_normType(normType),
            m_expNorm( (m_normType < std::numeric_limits<Real>::infinity()) ? normType : 1)
        {};    

        /**
         * Returns the weight of the figure
         */ 
        Real weight() const {return m_weight; }

        /**
         * Creates a new accumulator.
         * @param initialValue Initial accumulator value.
         */
        virtual Accumulator accumulator(Real initialValue) const override
        { return Accumulator(std::move(initialValue), m_normType); }

        /**
         * Returns a std::unique_ptr to an evaluator for the figure of merit. 
         */
        virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator() override
        {
            return std::make_unique<UniformityPropertyEvaluator>(this);
        }

        /** 
         * Returns the number fo equidistribution bits.
         */ 
        unsigned int nbBits() const {return m_nbBits; }

        /** 
         * Returns the exponent to use when accumulating merits
         */ 
        Real expNorm() const {return m_expNorm;}

    private:

        /** 
         * Evaluator class for UniformityProperty. 
         */
        class UniformityPropertyEvaluator : public FigureOfMeritEvaluator
        {
            public:
                /**
                 * Constructor. 
                 * @param figure Pointer to the figure of merit.
                 */
                UniformityPropertyEvaluator(UniformityProperty* figure):
                    m_figure(figure),
                    m_tmpReducer(m_figure->nbBits()),
                    m_memReducer(m_figure->nbBits()),
                    m_newReducer(m_figure->nbBits())
                {};

                /** 
                 * Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
                 *  starting from the initial value \c initialValue.
                 *  @param net is the net for which we compute the merit
                 *  @param dimension is the dimension for which we want to compute the merit
                 *  @param initialValue is the value from which to start
                 *  @param verbose controls the level of verbosity of the computation
                 */ 
                virtual MeritValue operator()(const DigitalNet& net, unsigned int dimension, MeritValue initialValue, int verbose = 0) override
                {

                    auto acc = m_figure->accumulator(std::move(initialValue)); // create the accumulator from the initial value

                    unsigned int size = m_figure->nbBits()*dimension;
                    net.extendSize(size,size);

                    m_newReducer = m_memReducer;

                    if (dimension>0)
                    {
                        for(unsigned int i = 0; i < m_figure->nbBits(); ++i)
                        {
                            GeneratingMatrix newCol(0,1);
                            for(unsigned int dim = 1; dim < dimension; ++dim)
                            {
                                newCol.stackBelow(net.pointerToGeneratingMatrix(dim)->subMatrix(0, m_figure->nbBits()*(dimension-1), m_figure->nbBits(), 1));
                            }
                            m_newReducer.addColumn(newCol);
                        }
                    }

                    for(unsigned int i = 0; i < m_figure->nbBits(); ++i)
                    {
                        GeneratingMatrix newRow = net.pointerToGeneratingMatrix(dimension)->subMatrix(i, 0, 1, m_figure->nbBits()*dimension);

                        m_newReducer.addRow(std::move(newRow));
                    }

                    if (m_newReducer.computeRank() < m_newReducer.numRows())
                    {
                        acc.accumulate(m_figure->weight(), 1, m_figure->expNorm());
                    }

                    if(!onProgress()(acc.value()))
                    {
                        acc.accumulate(std::numeric_limits<Real>::infinity(), 1, 1); // set the merit to infinity
                        onAbort()(net); // abort the computation
                    }

                    return acc.value();
                }

                /**     
                 * Resets the evaluator and prepare it to evaluate a new net.
                 */ 
                virtual void reset() override
                {
                    m_tmpReducer.reset(0);
                    m_memReducer.reset(0);
                    m_newReducer = m_memReducer;
                }

                /**
                 * Tells the evaluator that the last net was the best so far and store the relevant information
                 */
                virtual void lastNetWasBest() override
                {
                    m_tmpReducer = std::move(m_newReducer);
                }
                
                /**
                 * Tells the evaluator that no more net will be evaluate for the current dimension,
                 * store information about the best net for the dimension which is over and prepare data structures
                 * for the next dimension.
                 */ 
                virtual void prepareForNextDimension() override
                {
                    m_memReducer = m_tmpReducer;
                }



            private:
                UniformityProperty* m_figure;
                ProgressiveRowReducer m_tmpReducer;
                ProgressiveRowReducer m_memReducer;
                ProgressiveRowReducer m_newReducer;

        };

        unsigned int m_nbBits;
        Real m_weight;
        Real m_normType;
        Real m_expNorm;
};

/**
 * Special case of NetBuilder::UniformityProperty for one equidistribution bit. This figure was introduced by Sobol'
 * as the A-Property.
 */ 
class AProperty : public UniformityProperty {
    public:
        AProperty(Real weight = std::numeric_limits<Real>::infinity(), Real normType = std::numeric_limits<Real>::infinity()):
            UniformityProperty(1, weight, normType)
        {};
};

/**
 * Special case of NetBuilder::UniformityProperty for one equidistribution bit. This figure was introduced by Sobol'
 * as the A'-Property.
 */ 
class APrimeProperty : public UniformityProperty {
    public:
        APrimeProperty(Real weight = std::numeric_limits<Real>::infinity(), Real normType = std::numeric_limits<Real>::infinity()):
            UniformityProperty(2, weight, normType)
        {};
};

}}

#endif
