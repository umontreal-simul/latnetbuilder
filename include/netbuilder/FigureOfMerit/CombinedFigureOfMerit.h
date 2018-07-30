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

#ifndef NET_BUILDER__FIGURE_OF_MERIT_BIT__COMBINED_FIGURE_OF_MERIT_H
#define NET_BUILDER__FIGURE_OF_MERIT_BIT__COMBINED_FIGURE_OF_MERIT_H

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/LevelCombiner.h"

namespace NetBuilder { namespace FigureOfMerit {

using LatBuilder::Functor::AllOf;

/** 
 * Aggregation of figures of merit computed in a specific order.
 */ 
class CombinedFigureOfMerit : public CBCFigureOfMerit{

    public:

        /** 
         * Constructor.
         * @param normType Norm type use in the aggregation of merits
         * @param figures Vector of figures of merit
         * @param weights Vector of weights
         */ 
        CombinedFigureOfMerit(Real normType, std::vector<std::unique_ptr<CBCFigureOfMerit>> figures, std::vector<Real> weights):
            m_normType(normType),
            m_figures(std::move(figures)),
            m_size((unsigned int) m_figures.size()),
            m_weights(std::move(weights)),
            m_expNorm( (m_normType < std::numeric_limits<Real>::infinity()) ? normType : 1)
        {};

        /** 
         * Returns the norm type of the figure 
         */
        Real normType() const { return m_normType; }

        /**
         * Creates a new accumulator.
         * @param initialValue Initial accumulator value.
         */
        virtual Accumulator accumulator(Real initialValue) const override
        { return Accumulator(std::move(initialValue), m_normType); }

        /** 
         * Returns the vector of weights. 
         */
        std::vector<Real> weights() const { return m_weights; }

        /** 
         * Returns the number of figures. 
         */
        unsigned int size() const {return m_size ; }

        /** 
         Returns a pointer to the figure in position \c num. 
         */
        CBCFigureOfMerit* pointerToFigure(unsigned int num) const { return m_figures[num].get() ; }

        /**
         * Returns a <code>std::unique_ptr</code> to an evaluator for the figure of merit. 
         */
        virtual std::unique_ptr<CBCFigureOfMeritEvaluator> evaluator() override
        {
            return std::make_unique<CombinedFigureOfMeritEvaluator>(this);
        }

        /** 
         * Returns the exponent to use when accumulating merits
         */ 
        Real expNorm() const { return m_expNorm; }

        /**
         * Output information about the figure of merit.
         */ 
        virtual std::string format() const override
        {
            std::string res;
            res += "Combined figure of merit";
            for (unsigned int i=0; i<m_figures.size(); i++){
                res += "\n   Figure " + std::to_string(i) + ": " + m_figures[i]->format();
            }
            return res;
        }; 

    private:

        /** 
         * Evaluator class for CombinedFigureOfMerit. 
         */
        class CombinedFigureOfMeritEvaluator : public CBCFigureOfMeritEvaluator
        {
            public:
                /**
                 * Constructor. 
                 * @param figure Pointer to the figure of merit.
                 */
                CombinedFigureOfMeritEvaluator(CombinedFigureOfMerit* figure):
                    m_figure(figure),
                    m_oldMerits(figure->size(),0),
                    m_newMerits(figure->size(),0)
                {
                    for(unsigned int i = 0; i < m_figure->size(); ++i)
                    {
                        m_evaluators.push_back((m_figure->pointerToFigure(i)->evaluator()));
                    }
                };

                /** 
                 * Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
                 *  starting from the initial value \c initialValue.
                 *  @param net Net to evaluate.
                 *  @param dimension Dimension to compute.
                 *  @param initialValue Initial value of the merit.
                 *  @param verbose Verbosity level.
                 */ 
                virtual MeritValue operator()(const DigitalNet& net, Dimension dimension, MeritValue initialValue, int verbose = 0) override
                {
                    auto acc = m_figure->accumulator(0); // create the accumulator from the initial value

                    Real weight; // weight of the figure currently evaluated

                    // capture used to determine whether the computation should be aborted is early abortion is activated
                    auto goOn = [this, &acc, &weight] (MeritValue value) -> bool { return this->onProgress()(acc.tryAccumulate(weight, value, this->m_figure->expNorm())) ;} ;


                    for(unsigned int i = 0; i < m_figure->size(); ++i)
                    {
                        if (verbose>0)
                        {
                            std::cout << "Computing for figure num " << i  << "..." << std::endl;
                        }

                        weight = m_figure->weights()[i];

                        if (weight != 0.0)
                        {
                            auto goOnConnection = m_evaluators[i]->onProgress().connect(goOn); // connect the closure

                            m_newMerits[i] = (*m_evaluators[i])(net, dimension, 0, verbose-1); // compute the merit

                            acc.accumulate(m_figure->weights()[i], m_newMerits[i], m_figure->expNorm()) ; // accumulate the merit

                            goOnConnection.disconnect(); // disconnect the closure

                            if (verbose>0)
                            {
                                std::cout << "Partial merit value: " << acc.value() << std::endl;
                            }

                            if (!onProgress()(acc.value())) // if someone is listening, may tell that the computation is useless
                            {
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

                /**     
                 * Resets the evaluator and prepare it to evaluate a new net.
                 */ 
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

                /**
                 * Tells the evaluator that the last net was the best so far and store the relevant information
                 */
                virtual void prepareForNextDimension() override
                {
                    for(auto& eval : m_evaluators)
                    {
                        eval->prepareForNextDimension();
                    }
                    m_oldMerits = std::move(m_bestNewMerits);
                }

                /**
                 * Tells the evaluator that no more net will be evaluate for the current dimension,
                 * store information about the best net for the dimension which is over and prepare data structures
                 * for the next dimension.
                 */ 
                virtual void lastNetWasBest() override
                {
                    for(auto& eval : m_evaluators)
                    {
                        m_bestNewMerits = m_newMerits;
                        eval->lastNetWasBest();
                    }
                }

            private:
                CombinedFigureOfMerit* m_figure; // pointer to the figure
                std::vector<std::unique_ptr<CBCFigureOfMeritEvaluator>> m_evaluators; // evaluators
                std::vector<Real> m_oldMerits; // merits for the best net of the previous dimension
                std::vector<Real> m_bestNewMerits; // best merits for the best net so far for the current dimension
                std::vector<Real> m_newMerits; // merits of the latest evaluated net 

        };

        Real m_normType; // norm type of the figure
        std::vector<std::unique_ptr<CBCFigureOfMerit>> m_figures; // vector of aggregated figures
        unsigned int m_size; // number of figures
        std::vector<Real> m_weights; // individual weight of each aggregated figure
        Real m_expNorm; // exponent used in accumulation
};

}}

#endif
