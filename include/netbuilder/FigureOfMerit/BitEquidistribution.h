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

#ifndef NET_BUILDER_BIT_EQUIDISTRIBUTION
#define NET_BUILDER_BIT_EQUIDISTRIBUTION

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/ProgressiveRowReducer.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <limits>

#include <functional>
#include <boost/signals2.hpp>


namespace NetBuilder { namespace FigureOfMerit {

template <PointSetType PST>
class BitEquidistribution : public FigureOfMerit{

    public:

        /** Constructor.
         * @param weight is weight of the figure of merit;
         */ 
        BitEquidistribution(unsigned int nbBits, Real weight = std::numeric_limits<Real>::infinity(), Real normType = std::numeric_limits<Real>::infinity(), Combiner combiner = Combiner()):
            m_nbBits(nbBits),
            m_weight(weight),
            m_binOp(realToBinOp(normType)),
            m_normType(normType),
            m_expNorm( (m_normType < std::numeric_limits<Real>::infinity()) ? normType : 1),
            m_combiner(std::move(combiner))
        {};    

        Real weight() const {return m_weight; }

        Accumulator accumulator(Real initialValue) const
        { return Accumulator(std::move(initialValue), m_binOp); }

        /** Instantiates an evaluator and returns a std::unique_ptr to it. */
        virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator()
        {
            return std::make_unique<BitEquidistributionEvaluator>(this);
        }

        unsigned int nbBits() const {return m_nbBits; }

        Real expNorm() const {return m_expNorm;}

        Real combine(std::vector<unsigned int> merits)
        {
            RealVector tmp(merits.size());
            for(unsigned int i = 0; i < merits.size(); ++i)
            {
                tmp[i] = merits[i];
            }
            return m_combiner(std::move(tmp));
        }

    private:

        /** Evaluator class for BitEquidistribution. */
        class BitEquidistributionEvaluator : public FigureOfMeritEvaluator
        {
            public:
                /**Constructor. 
                 * @param figure is a pointer to the figure of merit.
                */
                BitEquidistributionEvaluator(BitEquidistribution* figure):
                    m_figure(figure),
                    m_tmpReducer(m_figure->nbBits()),
                    m_memReducer(m_figure->nbBits()),
                    m_newReducer(m_figure->nbBits())
                {};

                /** Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
                 *  starting from the initial value \c initialValue.
                 *  @param net is the net for which we compute the merit
                 *  @param dimension is the dimension for which we want to compute the merit
                 *  @param initialValue is the value from which to start
                 *  @param verbose controls the level of verbosity of the computation
                 */ 
                virtual MeritValue operator()(const DigitalNet& net, unsigned int dimension, MeritValue initialValue, int verbose = 0) override
                {
                    return initialValue;
                }

                virtual void reset() override
                {
                    m_tmpReducer.reset(0);
                    m_memReducer.reset(0);
                    m_newReducer = m_memReducer;
                }

                virtual void lastNetWasBest() override
                {
                    m_tmpReducer = std::move(m_newReducer);
                }
                
                virtual void prepareForNextDimension() override
                {
                    m_memReducer = m_tmpReducer;
                }

            private:
                BitEquidistribution* m_figure;
                ProgressiveRowReducer m_tmpReducer;
                ProgressiveRowReducer m_memReducer;
                ProgressiveRowReducer m_newReducer;

        };

        unsigned int m_nbBits;
        Real m_weight;
        BinOp m_binOp;
        Real m_normType;
        Real m_expNorm;
        Combiner m_combiner;
};

template<>
MeritValue BitEquidistribution<PointSetType::UNILEVEL>::BitEquidistributionEvaluator::operator()(const DigitalNet& net, unsigned int dimension, MeritValue initialValue, int verbose)
{

    unsigned int nCols = net.numColumns();
    if (dimension==1)
    {
        m_memReducer.reset(nCols);
    }

    auto acc = m_figure->accumulator(std::move(initialValue)); // create the accumulator from the initial value

    m_newReducer = m_memReducer;

    for(unsigned int bit = 0; bit < m_figure->nbBits(); ++bit)
    {
        m_newReducer.addRow(net.pointerToGeneratingMatrix(dimension)->subMatrix(bit, 1, nCols ));
        if (m_newReducer.computeRank() < m_newReducer.numRows())
        {
            acc.accumulate(m_figure->weight(), 1, m_figure->expNorm());
            break;
        }
    }

    if(!onProgress()(acc.value()))
    {
        acc.accumulate(std::numeric_limits<Real>::infinity(), 1, 1); // set the merit to infinity
        onAbort()(net); // abort the computation
    }

    return acc.value();
}

template<>
MeritValue BitEquidistribution<PointSetType::MULTILEVEL>::BitEquidistributionEvaluator::operator()(const DigitalNet& net, unsigned int dimension, MeritValue initialValue, int verbose)
{

    unsigned int nCols = net.numColumns();
    if (dimension==1)
    {
        m_memReducer.reset(nCols);
    }

    auto acc = m_figure->accumulator(std::move(initialValue)); // create the accumulator from the initial value

    m_newReducer = m_memReducer;

    std::vector<unsigned int> merits(nCols,0);

    for(unsigned int bit = 0; bit < m_figure->nbBits(); ++bit)
    {
        m_newReducer.addRow(net.pointerToGeneratingMatrix(dimension)->subMatrix(bit, 1, nCols ));
        std::vector<unsigned int> ranks = m_newReducer.computeRanks(0,nCols);

        for(unsigned int m = 1; m <= nCols; ++m)
        {
            if (m >= m_newReducer.numRows() && ranks[m-1] <  m_newReducer.numRows() )
            {
                merits[m-1] = 1;
            }
        }

        if (ranks[nCols-1] < nCols)
        {
            break;
        }
    }

    Real merit = m_figure->combine(merits);
    
    if (merit > 0)
    {
        acc.accumulate(m_figure->weight(), merit, m_figure->expNorm());
    }
    
    if(!onProgress()(acc.value()))
    {
        acc.accumulate(std::numeric_limits<Real>::infinity(), 1, 1); // set the merit to infinity
        onAbort()(net); // abort the computation
    }

    return acc.value();
}

}}

#endif
