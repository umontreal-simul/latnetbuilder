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

#ifndef NET_BUILDER_EQUIDISTRIBUTION_PROPERTY
#define NET_BUILDER_EQUIDISTRIBUTION_PROPERTY

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/ProgressiveRowReducer.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <string>

#include <functional>
#include <boost/signals2.hpp>


namespace NetBuilder { namespace FigureOfMerit {

using LatBuilder::Functor::AllOf;

class EquidistributionProperty : public FigureOfMerit{

    public:

        /** Constructor.
         * @param weight is weight of the figure of merit;
         */ 
        EquidistributionProperty(unsigned int nbBits, Real weight = std::numeric_limits<Real>::infinity(), BinOp binOp = OpMax()):
            m_nbBits(nbBits),
            m_weight(weight),
            m_binOp(binOp)
        {};    

        Real weight() const {return m_weight; }

        Accumulator accumulator(Real initialValue) const
        { return Accumulator(std::move(initialValue), m_binOp); }

        /** Instantiates an evaluator and returns a std::unique_ptr to it. */
        virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator()
        {
            return std::make_unique<EquidistributionPropertyEvaluator>(this);
        }

        unsigned int nbBits() const {return m_nbBits; }

    private:

        /** Evaluator class for EquidistributionProperty. */
        class EquidistributionPropertyEvaluator : public FigureOfMeritEvaluator
        {
            public:
                /**Constructor. 
                 * @param figure is a pointer to the figure of merit.
                */
                EquidistributionPropertyEvaluator(EquidistributionProperty* figure):
                    m_figure(figure)
                {};

                /** Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
                 *  starting from the initial value \c initialValue.
                 *  @param net is the net for which we compute the merit
                 *  @param dimension is the dimension for which we want to compute the merit
                 *  @param initialValue is the value from which to start
                 *  @param verbose controls the level of verbosity of the computation
                 */ 
                virtual MeritValue operator()(const DigitalNet& net, unsigned int dimension, MeritValue initialValue, int verbose = 0)
                {

                    assert(dimension == m_currentDim || dimension == m_currentDim+1);
                    if (dimension == m_currentDim+1){
                        m_currentDim++;
                        m_memReducer = m_tmpReducer;
                    }

                    //TO handle verbosity

                    auto acc = m_figure->accumulator(std::move(initialValue)); // create the accumulator from the initial value

                    auto grownNet = net.extendSize(m_figure->nbBits()*dimension);

                    auto newReducer = m_memReducer;

                    for(unsigned int i = 0; i < m_figure->nbBits(); ++i)
                    {
                        GeneratingMatrix newCol(0,1);
                        for(unsigned int dim = 1; dim < dimension; ++dim)
                        {
                            newCol.vstack(grownNet->pointerToGeneratingMatrix(dim)->subMatrix(0, m_figure->nbBits()*(dimension-1), m_figure->nbBits(), 1));
                        }
                        newReducer.addColumn(newCol);
                    }

                    GeneratingMatrix block = grownNet->pointerToGeneratingMatrix(dimension)->subMatrix(m_figure->nbBits(), m_figure->nbBits()*dimension);

                    if(!newReducer.reduceNewBlock(block))
                    {
                        acc.accumulate(m_figure->weight(),1,1);
                    }

                    if(!onProgress()(acc.value()))
                    {
                        acc.accumulate(std::numeric_limits<Real>::infinity(), 1, 1); // set the merit to infinity
                        onAbort()(net); // abort the computation
                        return acc.value();
                    }

                    m_tmpReducer = std::move(newReducer);
                    return acc.value();
                }

                void reset()
                {
                    m_currentDim = 0;
                    m_tmpReducer.reset(0);
                    m_memReducer.reset(0);
                }

            private:
                EquidistributionProperty* m_figure;
                unsigned int m_currentDim = 0;
                ProgressiveRowReducer m_tmpReducer;
                ProgressiveRowReducer m_memReducer;

        };

        unsigned int m_nbBits;
        Real m_weight;
        BinOp m_binOp;
};

class AProperty : public EquidistributionProperty {
    public:
        AProperty():
            EquidistributionProperty(1)
        {};
};

class APrimeProperty : public EquidistributionProperty {
    public:
        AProperty():
            EquidistributionProperty(2)
        {};
};

}}

#endif
