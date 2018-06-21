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

#ifndef NET_BUILDER__FIGURE_OF_MERIT_BIT__FIGURE_OF_MERIT_H
#define NET_BUILDER__FIGURE_OF_MERIT_BIT__FIGURE_OF_MERIT_H

#include "netbuilder/Types.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/Accumulator.h"

#include "latticetester/Weights.h"
#include "latticetester/Coordinates.h"

#include "latbuilder/Functor/AllOf.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <limits>
#include <functional>

#include <boost/signals2.hpp>


namespace NetBuilder { namespace FigureOfMerit {

using LatBuilder::Functor::AllOf;

/** 
 * Evaluator abstract class to evaluate figure of merit for a net.
 */ 
class FigureOfMeritEvaluator
{
    public:

        typedef boost::signals2::signal<bool (const MeritValue&), LatBuilder::Functor::AllOf> OnProgress;
        typedef boost::signals2::signal<void (const DigitalNet&)> OnAbort;

        /**
         * Default virtual destructor.
         */ 
        virtual ~FigureOfMeritEvaluator() = default;

        /**
        * Constructor.
        */
        FigureOfMeritEvaluator():
            m_onProgress(new OnProgress),
            m_onAbort(new OnAbort)
        {};

        /// \name Signals
        //@{
        /**
        * Progress signal.
        *
        * Emitted after a contribution to the
        * figure of merit is processed.
        * The signal argument is the cumulative value of the figure of
        * merit.  If any of the signal slots returns \c false, the computation of
        * the figure of merit will be aborted.
        */
        OnProgress& onProgress() const { return *m_onProgress; }

        /**
        * Abort signal.
        *
        * Emitted if the computation of the figure of merit is aborted.
        * The signal argument is the net for which the computation
        * was aborted.
        */
        OnAbort& onAbort() const { return *m_onAbort; }
        //@}

        /** 
         * Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
         * starting from the initial value \c initialValue.
         *  @param net Net to evaluate.
         *  @param dimension Dimension to compute.
         *  @param initialValue Initial value of the merit.
         *  @param verbose Verbosity level.
         */ 
        virtual MeritValue operator() (const DigitalNet& net, Dimension dimension, MeritValue initialValue, int verbose = 0) = 0;

        /** 
         * Computes the figure of merit for the given \c net for all the dimensions (full computation).
         * @param net Net to evaluate.
         * @param verbose Verbosity level.
         */ 
        MeritValue operator() (const DigitalNet& net, int verbose = 0)
        {
            MeritValue merit = 0; // start from a merit equal to zero
            for(Dimension dim = 0; dim < net.dimension(); ++dim) // for each dimension
            {
                prepareForNextDimension(); // prepare the evaluator for the next dimension
                if (verbose>0)
                {
                    std::cout << "Computing for dimension: " << dim << "..." <<std::endl;
                }
                merit = operator()(net, dim, merit, verbose-1); // evaluate the partial merit value
                if (verbose>0)
                {
                    std::cout << "Partial merit value: " << merit <<std::endl;
                }
                if (!onProgress()(merit)) // // if someone is listening, may tell that the computation is useless
                {
                    onAbort()(net);
                    merit = std::numeric_limits<Real>::infinity();
                    break;
                }
                lastNetWasBest(); // tell the evaluator that the only net evaluated was best and shoud be kept for the next dimension
            }
            reset(); // reset the evaluator
            return merit; 
        }

        /**     
         * Resets the evaluator and prepare it to evaluate a new net.
         */ 
        virtual void reset() = 0;

        /**
         * Tells the evaluator that no more net will be evaluate for the current dimension,
         * store information about the best net for the dimension which is over and prepare data structures
         * for the next dimension.
         */ 
        virtual void prepareForNextDimension() = 0;

        /**
         * Tells the evaluator that the last net was the best so far and store the relevant information
         */
        virtual void lastNetWasBest() = 0;

    private:
        std::unique_ptr<OnProgress> m_onProgress; 
        std::unique_ptr<OnAbort> m_onAbort;
};

/** 
 * Virtual class to represent any figure of merit. Derived classes should implement
 * the evaluator() member function returning a unique pointer to an instance of a concrete evaluator class 
 * which derives from FigureOfMeritEvaluator.
 */ 
class FigureOfMerit{

    public:

        /** 
         * Virtual default destructor. 
         */
        virtual ~FigureOfMerit() = default;

        /**
         * Returns a std::unique_ptr to an evaluator for the figure of merit. 
         */
        virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator() = 0  ;

        /**
         * Creates a new accumulator.
         * @param initialValue Initial accumulator value.
         */
        virtual Accumulator accumulator(Real initialValue) const = 0 ;
};

}}

#endif
