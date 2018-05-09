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

/** Base class for projection-dependent merits which only require the net and the projection.
 */ 
template <PointSetType PST>
class SimpleProjDepMerit;

/** Template specialization for projection-dependent merits which only require the net 
 * and the projection in the case of simple nets.
 */ 
template<>
class SimpleProjDepMerit<PointSetType::UNILEVEL>
{
    public:
    
        virtual ~SimpleProjDepMerit() = default;

        /** Returns the name of the projection-dependent merit.
         */ 
        std::string name() const { return m_name;}

        /**  Overload of << operator to print the name of the the projection-dependent merit on the given output stream
         */ 
        friend std::ostream& operator<<(std::ostream& os, const SimpleProjDepMerit& dt)
        {
            os << "Projection-dependent merit: " << dt.name();
            return os;
        } 

        /** Computes the projection-dependent figure of merit of the net for the given projection.
         * @param net is the digital net for which we want to compute the merit
         * @param projection is the projection to consider
         */ 
        virtual Real operator()(const DigitalNet& net, const LatCommon::Coordinates& projection) const {return 0 ; }

        std::vector<LatCommon::Coordinates> projections(unsigned int dimension) const
        { 
            std::vector<LatCommon::Coordinates> res(1);
            for(unsigned int i = 0; i < dimension; ++i)
            {
                res[0].insert(i);
            }
            return res;
        }

    protected:
        std::string m_name = "dummy"; // name of the projection-dependent merit
};

/** Template specialization for projection-dependent merits which only require the net and the projection in the case of 
 * embedded nets.
 */ 
template<>
class SimpleProjDepMerit<PointSetType::MULTILEVEL>
{
    public:

        virtual ~SimpleProjDepMerit() = default;

        /** Returns the name of the projection-dependent merit.
         */ 
        std::string name() const { return m_name ;}

        /**  Overload of << operator to print the name of the the projection-dependent merit on the given output stream
         */ 
        friend std::ostream& operator<<(std::ostream& os, const SimpleProjDepMerit& dt)
        {
            os << "Projection-dependent merit: " << dt.name();
            return os;
        } 

        /** Computes the projection-dependent figure of merit of the net for the given projection.
         * @param net is the digital net for which we want to compute the merit
         * @param projection is the projection to consider
         */ 
        virtual Real operator()(const DigitalNet& net, const LatCommon::Coordinates& projection) const {return 0; }

        std::vector<LatCommon::Coordinates> projections(unsigned int dimension) const
        { 
            std::vector<LatCommon::Coordinates> res(1);
            for(unsigned int i = 0; i < dimension; ++i)
            {
                res[0].insert(i);
            }

            return res;
        }

    protected:
        std::string m_name =  "Dummy"; // name of the projection-dependent merit
        std::function<Real (const RealVector&)> m_combiner = nullCombiner() ; // combiner for the merits of each level
};

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
            m_binOp(realToBinOp(normType))
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

    private:

        Real m_normType;
        std::unique_ptr<LatCommon::Weights> m_weights;
        std::unique_ptr<PROJDEP> m_projDepMerit;
        BinOp m_binOp;

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
            virtual MeritValue operator() (const DigitalNet& net, unsigned int dimension, MeritValue initialValue, bool verbose = true)
            {
                using namespace LatCommon;


                // TO DO : more explciit name and document interface for type of projections
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

                    if(verbose)
                    {
                        std::cout << "projection: " << proj << " - weight: " << weight << " - merit: " << merit << std::endl;
                    }

                    acc.accumulate(weight, merit, m_figure->normType()); // accumulate the merit

                    if (!onProgress()(acc.value())) { // if the current merit is too high
                        acc.accumulate(std::numeric_limits<Real>::infinity(), merit, m_figure->normType()); // set the merit to infinity
                        onAbort()(net); // abort the computation
                        break;
                    }
                }
                return acc.value(); // return the result
            } 

            private:
                WeightedFigureOfMerit* m_figure;
        };
};

}}

#endif 

