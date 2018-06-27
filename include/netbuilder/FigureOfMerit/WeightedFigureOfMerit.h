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

#ifndef NETBUILDER__FIGURE_OF_MERIT_BIT__WEIGHTED_FIGURE_OF_MERIT_H
#define NETBUILDER__FIGURE_OF_MERIT_BIT__WEIGHTED_FIGURE_OF_MERIT_H

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"

#include "latticetester/Coordinates.h"

namespace NetBuilder{ namespace FigureOfMerit { 

/** Class which represents a weighted figure of merit based on a projection dependent merit whose type is the template
 * parameter. 
 * 
 * @tparam PROJDEP The type of the projection dependent merit. This type should implement the following methods:
 *  - container<LatticeTester::Coordinates> projections(Dimension dimension) which returns an iterable container of the projections to consider for the 
 *  given dimension. \n 
 *  - Real operator()(const DigitalNet& net, LatticeTester::Coordinates) which returns the projection-dependent merit of the net for the given projection. \n
 *  
 */  
template<typename PROJDEP>
class WeightedFigureOfMerit : public FigureOfMerit
{
    public:

        /** Constructs a weigthed figure of merit based on the given arguments.
         * @param normType Norm type of the figure of merit.
         * @param weights Weights of the subprojections.
         * @param projDepMerit Projection-dependent merit.
         */  
        WeightedFigureOfMerit(Real normType, std::unique_ptr<LatticeTester::Weights> weights, std::unique_ptr<PROJDEP> projDepMerit):
            m_normType(normType),
            m_weights(std::move(weights)),
            m_projDepMerit(std::move(projDepMerit)),
            m_expNorm( (normType < std::numeric_limits<Real>::infinity()) ? normType : 1)
        {};

        /** 
         * Returns the weights of the figure 
         */
        const LatticeTester::Weights& weights() const { return *m_weights; }

        /** 
         * Returns the projection-dependent merit of the figure 
         */
        PROJDEP& projDepMerit() const { return *m_projDepMerit; }

        /** 
         * Returns the norm type of the figure .
         */
        Real normType() const{ return m_normType; }

        /**
         * Creates a new accumulator.
         * @param initialValue Initial accumulator value.
         */
        virtual Accumulator accumulator(Real initialValue) const override
        { return Accumulator(std::move(initialValue), m_normType); }

        /**
         * Returns a <code>std::unique_ptr</code> to an evaluator for the figure of merit. 
         */
        virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator() override
        {
            return std::make_unique<WeightedFigureOfMeritEvaluator>(this);
        }

        /** 
         * Returns the exponent to use when accumulating merits
         */ 
        Real expNorm() const { return m_expNorm; }

    private:

        Real m_normType; // norm type of the figure
        std::unique_ptr<LatticeTester::Weights> m_weights; // weights of the projections
        std::unique_ptr<PROJDEP> m_projDepMerit; // projection dependent merit
        Real m_expNorm; // exponent to use when accumulating merit

        /** 
         * Class which describes how the figure of merit is computed. 
         */
        class WeightedFigureOfMeritEvaluator : public FigureOfMeritEvaluator
        {
            public:
                /** Constructs the evaluator */
                WeightedFigureOfMeritEvaluator(WeightedFigureOfMerit* figure):
                    m_figure(figure)
                {};

                /** 
                 * Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
                 * starting from the initial value \c initialValue.
                 *  @param net Net to evaluate.
                 *  @param dimension Dimension to compute.
                 *  @param initialValue Initial value of the merit.
                 *  @param verbose Verbosity level.
                 */ 
                virtual MeritValue operator() (const DigitalNet& net, Dimension dimension, MeritValue initialValue, int verbose = 0) override
                {
                    using namespace LatticeTester;
                    std::cout << "hi"<<std::endl;
                    auto projections = m_figure->projDepMerit().projections(dimension);
                    std::cout << "ho"<<std::endl;
                    auto acc = m_figure->accumulator(std::move(initialValue)); // create the accumulator from the initial value

                    for (auto cit = projections.begin (); cit != projections.end (); ++cit) // for each coordinate
                    {
                        const Coordinates& proj = *cit;
                        std::cout << "projection: " << proj << std::endl;
                        Real weight = m_figure->weights().getWeight(proj); // get the weight

                        if (weight == 0.0) { 
                            continue; // skip the projection
                        }

                        MeritValue merit = m_figure->projDepMerit()(net, proj); // compute the proj-dep merit

                        if(verbose>0)
                        {
                            std::cout << "projection: " << proj << " - weight: " << weight << " - merit: " << merit << std::endl;
                        }

                        acc.accumulate(weight, merit, m_figure->expNorm()); // accumulate the merit

                        if (!onProgress()(acc.value())) { // if the current merit is too high
                            acc.accumulate(std::numeric_limits<Real>::infinity(), merit, m_figure->expNorm()); // set the merit to infinity
                            onAbort()(net); // abort the computation
                            break;
                        }
                    }
                    return acc.value(); // return the result
                }

                /**     
                 * Resets the evaluator and prepare it to evaluate a new net.
                 */ 
                virtual void reset() override {};

                /**
                 * Tells the evaluator that no more net will be evaluate for the current dimension,
                 * store information about the best net for the dimension which is over and prepare data structures
                 * for the next dimension.
                 */ 
                virtual void prepareForNextDimension() override {}; 

                /**
                 * Tells the evaluator that the last net was the best so far and store the relevant information
                 */
                virtual void lastNetWasBest() override {};

            private:

                WeightedFigureOfMerit* m_figure;
        };
};

}}

#endif 

