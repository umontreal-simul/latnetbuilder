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

/**
 * \file
 * This file contains the definition of merit accumulators.
 */ 

#ifndef NETBUILDER__ACCUMULATOR_H
#define NETBUILDER__ACCUMULATOR_H

#include "netbuilder/Types.h"

#include <functional>

namespace NetBuilder{

/**
 * Add binary operation class.
 */ 
struct AddBinaryOperator
{
    Real operator()(Real x, Real y);
};

/**
 * Max binary operation class.
 */
struct MaxBinaryOperator
{
    Real operator()(Real x, Real y);
};

/**
 * Accumulator class. 
 * Used by figures of merit to aggregate partial merit values (dimension, projection-dependent, etc.).
 */ 
class Accumulator
{

    public:

        /**
         * Constructor.
         * @param initialValue Initial value of the accumulator.
         * @param normType Norm type of the accumulator. A finite real corresponds to the q-norm 
         * whereas the infinite corresponds to the sup norm.
         */ 
        Accumulator(Real initialValue, Real normType);

        /**
         * Default copy constructor.
         */ 
        Accumulator(Accumulator&) = default;

        /**
         * Default move constructor.
         */ 
        Accumulator(Accumulator&&) = default;

        /**
         * Accumulate a new merit value \c value with weight \c weight. The accumulator raises \c value to power \c power.
         * @param weight Weight of the \c value.
         * @param value Value to accumulate.
         * @param power Power used when raising \c value.
         */
        void accumulate(Real weight, Real value, Real power);

        /**
         * Returns the value which would be held by the accumulator
         * if a new merit value \c value with weight \c weight were accumulated. The accumulator raises \c value to power \c power.
         * @param weight Weight of the \c value.
         * @param value Value to accumulate.
         * @param power Power used when raising \c value.
         */
        Real tryAccumulate(Real weight, Real value, Real power) const;

        /**
         * Set the current merit value held by the accumulator to \c value.
         */ 
        void set(Real value);

        /**
         * Returns the current merit value held by the accumulator.
         */ 
        Real value() const;


    private:

        Real m_data; // current value
        std::function<Real (Real, Real)>  m_op; // binary opration
        
        /**
         * Binds a norm-type \c normType to the corresponding binary operation.
         */ 
        static std::function<Real (Real, Real)>  realToBinOp(Real normType);
};

}

#endif