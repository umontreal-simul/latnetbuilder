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
#include "netbuilder/FigureOfMerit.h"

#include "latcommon/Weights.h"

#include "latbuilder/Functor/AllOf.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <string>

#include <boost/function.hpp>
#include <boost/signals2.hpp>


namespace NetBuilder {

using LatBuilder::Functor::AllOf;

class CombinedFigureOfMerit : public FigureOfMerit{

    public:

        CombinedFigureOfMerit(Real normType, std::vector<std::unique_ptr<FigureOfMerit>> figures, std::vector<Real> weights, BinOp binOp):
            m_normType(normType),
            m_figures(std::move(figures)),
            m_size(m_figures.size()),
            m_binOp(binOp),
            m_weights(std::move(weights))
        {};

        /** Returns the norm type of the figure */
        Real normType() const { return m_normType; }

        /**
         * Creates a new accumulator.
         * @param initialValue Initial accumulator value.
         */
        Accumulator accumulator(Real initialValue) const
        { return Accumulator(std::move(initialValue), m_binOp); }

        std::vector<Real> weights() const { return m_weights; }

        unsigned int size() const {return m_size ; }

        FigureOfMerit* figure(unsigned int num) const { return m_figures[num].get() ; }

        virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator()
        {
            return std::make_unique<CombinedFigureOfMeritEvaluator>(this);
        }

    private:

        class CombinedFigureOfMeritEvaluator : public FigureOfMeritEvaluator
        {
            public:
                CombinedFigureOfMeritEvaluator(CombinedFigureOfMerit* figure):
                    m_figure(figure)
                {
                    for(unsigned int i = 0; i < m_figure->size(); ++i)
                    {
                        m_evaluators.push_back(std::move(m_figure->figure(i)->evaluator()));
                    }
                };

                virtual MeritValue operator()(const DigitalNet& net, unsigned int dimension, MeritValue initialValue, bool verbose = false)
                {
                    auto acc = m_figure->accumulator(std::move(initialValue)); // create the accumulator from the initial value

                    Real weight;

                    auto goOn = [this, &acc, &weight] (MeritValue value) -> bool { return this->onProgress()(acc.tryAccumulate(weight, value, this->m_figure->normType())) ;} ;

                    auto abort = [this] (const DigitalNet& net) -> void { this->onAbort()(net) ;} ;

                    for(unsigned int i = 0; i < m_figure->size(); ++i)
                    {
                        weight = m_figure->weights()[i];

                        if (weight != 0.0)
                        {
                            auto goOnConnection = m_evaluators[i]->onProgress().connect(goOn);
                            auto abortConnection = m_evaluators[i]->onAbort().connect(abort);

                            MeritValue merit = (*m_evaluators[i])(net, dimension, 0, verbose);

                            acc.accumulate(m_figure->weights()[i], merit, m_figure->normType()) ;

                            goOnConnection.disconnect();
                            abortConnection.disconnect();

                            if (!onProgress()(acc.value())) { // if the current merit is too high
                                acc.accumulate(std::numeric_limits<Real>::infinity(), merit, m_figure->normType()); // set the merit to infinity
                                onAbort()(net); // abort the computation
                                break;
                            }
                        }
                    }
                    return acc.value();
                }

            private:
                CombinedFigureOfMerit* m_figure;
                std::vector<std::unique_ptr<FigureOfMeritEvaluator>> m_evaluators;

        };

        std::vector<std::unique_ptr<FigureOfMerit>> m_figures;
        unsigned int m_size;
        std::vector<Real> m_weights;
        Real m_normType;
        BinOp m_binOp;
};

}

#endif
