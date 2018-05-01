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

#ifndef NET_BUILDER_FIGURE_OF_MERIT_H
#define NET_BUILDER_FIGURE_OF_MERIT_H

#include "netbuilder/Types.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"

#include "latcommon/Weights.h"
#include "latcommon/Coordinates.h"

#include "latbuilder/Functor/AllOf.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <string>

#include <functional>
#include <boost/signals2.hpp>


namespace NetBuilder { namespace FigureOfMerit {

using LatBuilder::Functor::AllOf;

/** Evaluator class which evaluates a figure of merit for a net.
 */ 
class FigureOfMeritEvaluator
{
    public:
        typedef LatCommon::Coordinates Coordinates;

        typedef boost::signals2::signal<bool (const MeritValue&), LatBuilder::Functor::AllOf> OnProgress;
        typedef boost::signals2::signal<void (const DigitalNet&)> OnAbort;

        virtual ~FigureOfMeritEvaluator() = default;

        /**
        * Constructor.
        */
        FigureOfMeritEvaluator():
            m_onProgress(new OnProgress),
            m_onAbort(new OnAbort)
        {};

        /**
        * Move constructor.
        */
        FigureOfMeritEvaluator(FigureOfMeritEvaluator&&) = default;


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

        /** Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
         * starting from the initial value \c initialValue.
         * @param net is the net for which we compute the merit
         * &param dimension is the dimension for which we want to compute the merit
         * @param initialValue is the value from which to start
         * @param verbose controls the level of verbosity of the computation
         */ 
        virtual MeritValue operator() (const DigitalNet& net, unsigned int dimension, MeritValue initialValue, bool verbose = false) = 0;

        /** Computes the figure of merit for the given \c net for all the dimensions (full computation).
         * @param net is the net for which we compute the merit
         * @param verbose controls the level of verbosity of the computation
         */ 
        MeritValue operator() (const DigitalNet& net, bool verbose = false)
        {
            MeritValue merit = 0;
            for(unsigned int dim = 1; dim <= net.dimension(); ++dim)
            {
                merit = operator()(net, dim, merit);
            }
            reset();
            return merit;
        }

        /** Reset the evaluator, enabling the full computation for a new net
         */ 
        virtual void reset() { return;}


    private:
        std::unique_ptr<OnProgress> m_onProgress; 
        std::unique_ptr<OnAbort> m_onAbort;
};

/** Virtual class to represent any figure of merit. Derived classes should implement
 * the evaluator() member function and a concrete evaluator class which derives from FigureOfMeritEvaluator.
 */ 
class FigureOfMerit{

    public:

        /** Virtual default destructor. */
        virtual ~FigureOfMerit() = default;

        /* Returns a std::unique_ptr to an evaluator for the figure of merit. */
        virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator() = 0  ;
};

}}

#endif
