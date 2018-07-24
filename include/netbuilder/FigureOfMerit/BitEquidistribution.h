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

#ifndef NET_BUILDER__FIGURE_OF_MERIT__BIT_EQUIDISTRIBUTION_H
#define NET_BUILDER__FIGURE_OF_MERIT__BIT_EQUIDISTRIBUTION_H

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/ProgressiveRowReducer.h"



namespace NetBuilder { namespace FigureOfMerit {

/** Bit equidistribution figure.
 * 
 *  Figure of merit which penalizes digital nets for which the first bits are not equidistributed.
 */ 
template <EmbeddingType ET>
class BitEquidistribution : public CBCFigureOfMerit{

    public:

        typedef std::unique_ptr<LevelCombiner::LevelCombiner> pCombiner;

        /** 
         * Constructor.
         * @param nbBits Number of bits taken into account by the figure.
         * @param weight Weight of the figure of merit. Defaults to +inf.
         * @param normType Norm type of the figure of merit. Defaults to sup norm.
         * @param combiner LevelCombiner for the multilevel case. Default to the (non-callable) empty function.
         */ 
        BitEquidistribution(unsigned int nbBits, Real weight = std::numeric_limits<Real>::infinity(), Real normType = std::numeric_limits<Real>::infinity(), pCombiner combiner =std::make_unique<LevelCombiner::LevelCombiner>()):
            m_nbBits(nbBits),
            m_weight(weight),
            m_normType(normType),
            m_expNorm( (m_normType < std::numeric_limits<Real>::infinity()) ? normType : 1),
            m_combiner(std::move(combiner))
        {};    

        /**
         * Returns the weight of the figure of merit.
         */ 
        Real weight() const {return m_weight; }

        /**
         * Creates a new accumulator.
         * @param initialValue Initial accumulator value.
         */
        virtual Accumulator accumulator(Real initialValue) const override
        { return Accumulator(std::move(initialValue), m_normType); }

        /**
         * Returns a <code>std::unique_ptr</code> to an evaluator for the figure of merit. 
         */
        virtual std::unique_ptr<CBCFigureOfMeritEvaluator> evaluator() override
        {
            return std::make_unique<BitEquidistributionEvaluator>(this);
        }

        /** 
         * Returns the number of bits taken into account for equidistribution by the figure.
         */ 
        unsigned int nbBits() const {return m_nbBits; }

        /** 
         * Returns the exponent to use when accumulating merits
         */ 
        Real expNorm() const {return m_expNorm;}

        /** 
         * Returns a combined merit from a vector of merits using the combiner of the figure.
         */ 
        Real combine(std::vector<unsigned int> merits)
        {
            RealVector tmp(merits.size());
            for(unsigned int i = 0; i < merits.size(); ++i)
            {
                tmp[i] = merits[i];
            }
            return (*m_combiner)(std::move(tmp));
        }

        /**
         * Output information about the figure of merit.
         */ 
        virtual std::string format() const override
        {
            return "";
        }        


    private:

        /** 
         * Evaluator class for BitEquidistribution. 
         */
        class BitEquidistributionEvaluator : public CBCFigureOfMeritEvaluator
        {
            public:
                /**
                 * Constructor. 
                 * @param figure Pointer to the figure of merit.
                */
                BitEquidistributionEvaluator(BitEquidistribution* figure):
                    m_figure(figure),
                    m_tmpReducer(m_figure->nbBits()),
                    m_memReducer(m_figure->nbBits()),
                    m_newReducer(m_figure->nbBits())
                {};

                /** 
                 * Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
                 *  starting from the initial value \c initialValue. Intended to be specialized based on template parameter ET.
                 *  @param net Net to evaluate.
                 *  @param dimension Dimension to compute.
                 *  @param initialValue Initial value of the merit.
                 *  @param verbose Verbosity level.
                 */ 
                virtual MeritValue operator()(const DigitalNet& net, Dimension dimension, MeritValue initialValue, int verbose = 0) override
                {
                    return initialValue;
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
                BitEquidistribution* m_figure; // pointer to the figure of merit
                ProgressiveRowReducer m_tmpReducer; // contains the reduction for the best net so far
                ProgressiveRowReducer m_memReducer; // contains the reduction for the best net of the previous dimension
                ProgressiveRowReducer m_newReducer; // contains the reduction for the latest evaluated net

        };

        unsigned int m_nbBits; // number of equidistribution bits
        Real m_weight; // weight of the figure
        Real m_normType; // norm type of the figure
        Real m_expNorm; // exponent used in accumulation
        pCombiner m_combiner; // combiner used in the multilevel case
};


/**
 * Template specialization of the output information about the figure of merit in the unilevel case.
 */ 
template <>
std::string BitEquidistribution<EmbeddingType::UNILEVEL>::format() const
{            
    std::string res;
    res += std::to_string(m_nbBits) + "-bit equidistribution\nNorm type: " + std::to_string(m_normType);
    res += "\nEmbedding type: Unilevel";
    return res;
}; 

/**
 * Template specialization of the output information about the figure of merit in the multilevel case.
 */ 
template <>
std::string BitEquidistribution<EmbeddingType::MULTILEVEL>::format() const
{            
    std::string res;
    res += std::to_string(m_nbBits) + "-bit equidistribution\nNorm type: " + std::to_string(m_normType);
    res += "\nEmbedding type: Multilevel";
    res += "\nCombiner: " + this->m_combiner->format();
    return res;
};

// template specialization for the unilevel case
template<>
MeritValue BitEquidistribution<EmbeddingType::UNILEVEL>::BitEquidistributionEvaluator::operator()(const DigitalNet& net, Dimension dimension, MeritValue initialValue, int verbose)
{

    unsigned int nCols = net.numColumns();
    if (dimension==0)
    {
        m_memReducer.reset(nCols); // if the dimension is the first dimension, initiate the data structure
    }

    auto acc = m_figure->accumulator(std::move(initialValue)); // create the accumulator from the initial value

    m_newReducer = m_memReducer; // copy the reduction of the best net for the previous dimension

    for(unsigned int bit = 0; bit < m_figure->nbBits(); ++bit) // for each bit of equidistribution
    {
        m_newReducer.addRow(net.generatingMatrix(dimension).subMatrix(bit, 0, 1, nCols )); // add the new row
        if (m_newReducer.computeRank() < m_newReducer.numRows())
        {
            acc.accumulate(m_figure->weight(), 1, m_figure->expNorm()); // the points are not equidistributed: set the merit
            break;
        }
    }

    if(!onProgress()(acc.value())) // if someone is listening, may tell that the computation is useless
    {
        acc.accumulate(std::numeric_limits<Real>::infinity(), 1, 1); // set the merit to infinity
        onAbort()(net); // abort the computation
    }

    return acc.value();
}

//template specialization for the multilevel case
template<>
MeritValue BitEquidistribution<EmbeddingType::MULTILEVEL>::BitEquidistributionEvaluator::operator()(const DigitalNet& net, Dimension dimension, MeritValue initialValue, int verbose)
{

    unsigned int nCols = net.numColumns();

    if (dimension==0) // if the dimension is the first dimension, initiate the data structure
    {
        m_memReducer.reset(nCols);
    }

    auto acc = m_figure->accumulator(std::move(initialValue)); // create the accumulator from the initial value

    m_newReducer = m_memReducer; // copy the reduction of the best net for the previous dimension

    std::vector<unsigned int> merits(nCols,0);

    for(unsigned int bit = 0; bit < m_figure->nbBits(); ++bit) // for each bit of equidistribution
    {
        m_newReducer.addRow(net.generatingMatrix(dimension).subMatrix(bit, 0, 1, nCols )); // add the new row
        std::vector<unsigned int> ranks = m_newReducer.computeRanks(0,nCols); // compute the rank

        for(unsigned int m = 1; m <= nCols; ++m) // for each level of points
        {
            if (m >= m_newReducer.numRows() && ranks[m-1] <  m_newReducer.numRows() ) // if the system is not full row-rank and could have been
            {
                merits[m-1] = 1; // the points could have been equidistributed but are not: put the merit to 1
            }
        }

        if (ranks[nCols-1] < nCols)
        {
            break; // if even the system with most columns is not invertible, stop the computation
        }
    }

    Real merit = m_figure->combine(merits); // combine the merits
    
    if (merit > 0)
    {
        acc.accumulate(m_figure->weight(), merit, m_figure->expNorm()); // accumulate the merit
    }
    
    if(!onProgress()(acc.value())) // if someone is listening, may tell that the computation is useless
    {
        acc.accumulate(std::numeric_limits<Real>::infinity(), 1, 1); // set the merit to infinity
        onAbort()(net); // abort the computation
    }

    return acc.value();
}

}}

#endif
