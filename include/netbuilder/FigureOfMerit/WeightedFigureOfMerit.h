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

#ifndef NETBUILDER__WEIGHTED_FIGURE_OF_MERIT_H
#define NETBUILDER__WEIGHTED_FIGURE_OF_MERIT_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"

#include "latcommon/Weights.h"
#include "latcommon/Coordinates.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <functional>

namespace NetBuilder{ namespace FigureOfMerit { 

/** Class which represents a weighted figure of merit based on a projection dependent merit whose type is the template
 * parameter. 
 */  
template<typename PROJDEP>
class WeightedFigureOfMerit : public FigureOfMerit
{
    public:

        /** Constructs a weigthed figure of merit based on the given arguments.
         * @param normType is the power to which the projection-dependent merit should be raised
         * @param weights is a smart pointer to a Weights instance
         * @param projDepMerit is a smart pointer to the projection-dependent merit
         */  
        WeightedFigureOfMerit(Real normType, std::unique_ptr<LatCommon::Weights> weights, std::unique_ptr<PROJDEP> projDepMerit):
            m_normType(normType),
            m_weights(std::move(weights)),
            m_projDepMerit(std::move(projDepMerit)),
            m_binOp(realToBinOp(normType)),
            m_expNorm( (normType < std::numeric_limits<Real>::infinity()) ? normType : 1)
        {};

        /** Returns the weights of the figure */
        const LatCommon::Weights& weights() const { return *m_weights; }

        /** Returns the projection-dependent merit of the figure */
        PROJDEP& projDepMerit() const { return *m_projDepMerit; }

        /** Returns the norm type of the figure */
        Real normType() const{ return m_normType; }

        /**
         * Creates a new accumulator.
         * @param initialValue Initial accumulator value.
         */
        Accumulator accumulator(Real initialValue) const
        { return Accumulator(std::move(initialValue), m_binOp); }

        /** Instantiates an evaluator for the figure of merit and returns a smart pointer to it.
         */
        virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator()
        {
            return std::make_unique<WeightedFigureOfMeritEvaluator>(this);
        }

        Real expNorm() const { return m_expNorm; }

    private:

        Real m_normType;
        std::unique_ptr<LatCommon::Weights> m_weights;
        std::unique_ptr<PROJDEP> m_projDepMerit;
        BinOp m_binOp;
        Real m_expNorm;

        /** Class which describes how the figure of merit is computed. */
        class WeightedFigureOfMeritEvaluator : public FigureOfMeritEvaluator
        {
            public:
                /** Constructs the evaluator */
                WeightedFigureOfMeritEvaluator(WeightedFigureOfMerit* figure):
                    m_figure(figure)
                {};

            /** Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
             * starting from the initial value \c initialValue.
             * @param net is the net for which we compute the merit
             * @param dimension is the dimension for which we want to compute the merit
             * @param initialValue is the value from which to start
             * @param verbose controls the level of verbosity of the computation
             */ 
            virtual MeritValue operator() (const DigitalNet& net, unsigned int dimension, MeritValue initialValue, int verbose = 0) override
            {
                using namespace LatCommon;

                auto projections = m_figure->projDepMerit().projections(dimension);

                auto acc = m_figure->accumulator(std::move(initialValue)); // create the accumulator from the initial value

                for (auto cit = projections.begin (); cit != projections.end (); ++cit) // for each coordinate
                {
                    const Coordinates& proj = *cit;

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
                onComputationDone()(acc.value());
                return acc.value(); // return the result
            }

            virtual void reset() override {};

            virtual void prepareForNextDimension() override {}; 

            private:
                WeightedFigureOfMerit* m_figure;
        };
};

}}

#endif 

