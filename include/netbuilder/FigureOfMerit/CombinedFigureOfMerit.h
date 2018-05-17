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

#ifndef NET_BUILDER_COMBINED_FIGURE_OF_MERIT_H
#define NET_BUILDER_COMBINED_FIGURE_OF_MERIT_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"

#include "latcommon/Weights.h"

#include "latbuilder/Functor/AllOf.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <string>

#include <functional>
#include <boost/signals2.hpp>


namespace NetBuilder { namespace FigureOfMerit {

using LatBuilder::Functor::AllOf;

/** Class to describe an aggregation of figures of merit computed in a specific order.
 */ 
class CombinedFigureOfMerit : public FigureOfMerit{

    public:

        /** Constructor.
         * @param normType is the power to which to raise the aggregated merits
         * @param figures is a vector of std::unique_ptr to the figures of merit to aggregate
         * @param weights is a vector of weights to use to aggregate the figures of merit
         */ 
        CombinedFigureOfMerit(Real normType, std::vector<std::unique_ptr<FigureOfMerit>> figures, std::vector<Real> weights):
            m_normType(normType),
            m_figures(std::move(figures)),
            m_size((unsigned int) m_figures.size()),
            m_binOp(realToBinOp(normType)),
            m_weights(std::move(weights)),
            m_expNorm( (m_normType < std::numeric_limits<Real>::infinity()) ? normType : 1)
        {};

        /** Returns the norm type of the figure */
        Real normType() const { return m_normType; }

        /**
         * Creates a new accumulator.
         * @param initialValue is the initial accumulator value.
         */
        Accumulator accumulator(Real initialValue) const
        { return Accumulator(std::move(initialValue), m_binOp); }

        /** Returns the vector of weights. */
        std::vector<Real> weights() const { return m_weights; }

        /** Returns the number of figures. */
        unsigned int size() const {return m_size ; }

        /** Returns the figure in position \c num. */
        FigureOfMerit* figure(unsigned int num) const { return m_figures[num].get() ; }

        /** Instantiates an evaluator and returns a std::unique_ptr to it. */
        virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator()
        {
            return std::make_unique<CombinedFigureOfMeritEvaluator>(this);
        }

        Real expNorm() const { return m_expNorm; }

    private:

        /** Evaluator class for CombinedFigureOfMerit. */
        class CombinedFigureOfMeritEvaluator : public FigureOfMeritEvaluator
        {
            public:
                /**Constructor. 
                 * @param figure is a pointer to the figure of merit.
                */
                CombinedFigureOfMeritEvaluator(CombinedFigureOfMerit* figure):
                    m_figure(figure),
                    m_oldMerits(figure->size(),0),
                    m_newMerits(figure->size(),0)
                {
                    for(unsigned int i = 0; i < m_figure->size(); ++i)
                    {
                        m_evaluators.push_back((m_figure->figure(i)->evaluator()));
                    }
                };

                /** Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
                 *  starting from the initial value \c initialValue.
                 *  @param net is the net for which we compute the merit
                 *  @param dimension is the dimension for which we want to compute the merit
                 *  @param initialValue is the value from which to start
                 *  @param verbose controls the level of verbosity of the computation
                 */ 
                virtual MeritValue operator()(const DigitalNet& net, unsigned int dimension, MeritValue initialValue, int verbose = 0) override
                {
                    auto acc = m_figure->accumulator(std::move(0)); // create the accumulator from the initial value

                    Real weight;

                    auto goOn = [this, &acc, &weight] (MeritValue value) -> bool { return this->onProgress()(acc.tryAccumulate(weight, value, this->m_figure->expNorm())) ;} ;
                    for(unsigned int i = 0; i < m_figure->size(); ++i)
                    {
                        if (verbose>0)
                        {
                            std::cout << "Computing for figure n°" << i  << "..." << std::endl;
                        }
                        weight = m_figure->weights()[i];

                        if (weight != 0.0)
                        {
                            auto goOnConnection = m_evaluators[i]->onProgress().connect(goOn);

                            m_newMerits[i] = (*m_evaluators[i])(net, dimension, 0, verbose-1);

                            acc.accumulate(m_figure->weights()[i], m_newMerits[i], m_figure->expNorm()) ;

                            goOnConnection.disconnect();

                            if (verbose>0)
                            {
                                std::cout << "Partial merit value: " << acc.value() << std::endl;
                            }

                            if (!onProgress()(acc.value())) 
                            { // if the current merit is too high
                                acc.accumulate(weight, std::numeric_limits<Real>::infinity(), m_figure->expNorm()); // set the merit to infinity
                                onAbort()(net); // abort the computation
                                break;
                            }
                        }
                        else
                        {
                            if (verbose>0)
                            {
                                std::cout << "Skipping figure (zero weight): " << acc.value() << std::endl;
                            }
                        }
                    }
                    return acc.value();
                }

                virtual void reset() override
                {
                    for(auto& eval : m_evaluators)
                    {
                        eval->reset();
                    }
                    m_newMerits = std::vector<Real>(m_figure->size(),0);
                    m_bestNewMerits = std::vector<Real>(m_figure->size(),0);
                    m_oldMerits = std::vector<Real>(m_figure->size(),0);
                }

                virtual void prepareForNextDimension() override
                {
                    for(auto& eval : m_evaluators)
                    {
                        eval->prepareForNextDimension();
                    }
                    m_oldMerits = std::move(m_bestNewMerits);
                }

                virtual void lastNetWasBest() override
                {
                    for(auto& eval : m_evaluators)
                    {
                        m_bestNewMerits = m_newMerits;
                        eval->lastNetWasBest();
                    }
                }

            private:
                CombinedFigureOfMerit* m_figure;
                std::vector<std::unique_ptr<FigureOfMeritEvaluator>> m_evaluators;
                std::vector<Real> m_oldMerits;
                std::vector<Real> m_bestNewMerits;
                std::vector<Real> m_newMerits;

        };

        Real m_normType;
        std::vector<std::unique_ptr<FigureOfMerit>> m_figures;
        unsigned int m_size;
        BinOp m_binOp;
        std::vector<Real> m_weights;
        Real m_expNorm;
};

}}

#endif
