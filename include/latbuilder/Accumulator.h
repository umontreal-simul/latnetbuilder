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

#ifndef LATBUILDER__ACCUMULATOR_H
#define LATBUILDER__ACCUMULATOR_H

#include "latbuilder/Types.h"
#include "latbuilder/Vectorize.h"
#include "latbuilder/Functor/binary.h"

namespace LatBuilder {

/**
 * Accumulator template class.
 *
 * An accumulator holds a value that can be updated by feeding it with new
 * values with the accumulate() member function.  The scalar binary operation
 * by which new values are combined with the current value is specified with
 * the OP template parameter.  If the values are vectorial (indexable with the
 * [] operator), the operator OP is applied to each element of the vector
 * through the LatBuilder::Vectorize class template.
 *
 * \tparam OP              Class template that takes that provides the
 *                         implementation of a scalar binary operator.  Must
 *                         implement the static member functions \c name(),
 *                         that returns a string that identifies the operator,
 *                         and \c apply(current_accumulator_value,new_value),
 *                         which returns the new accumulator value when a value
 *                         \c new_value is fed to the accumulator while the
 *                         current accumulator value is
 *                         \c current_accumulator_value.
 *                         For examples, see Functor::Sum::name() and
 *                         Functor::Sum::apply().
 * \tparam VAL             Type of the values that are fed to the accumulator.
 * \tparam ACCVAL          Type of the accumulator value (in most cases it
 *                         should be the same as VAL).
 *
 * \see Functor::Sum Functor::Max
 */
template <template <typename> class OP, typename VAL, typename ACCVAL=VAL>
class Accumulator {
public:

   /**
    * Type of the values fed to the accumulator.
    */
   typedef VAL value_type;

   /**
    * Type of the accumulator value.
    */
   typedef ACCVAL accumulator_value_type;

   Accumulator(accumulator_value_type initialValue):
      m_value(initialValue)
   {}

   /**
    * Feeds a new value to the accumulator.
    */
   void accumulate(const value_type& value)
   { accumulate(1.0, value); }

   /**
    * Feeds \c value multiplied by \c weight to the accumulator.
    * \c weight is assumed to already be raised at the power \c power.
    */
   void accumulate(Real weight, const value_type& value, Real power = 1.0)
   { m_value = Vectorize::apply<OP<Real>>(m_value, weight * Vectorize::apply<Functor::Pow>(value, power)); }

   /**
    * Returns the current value of the accumulator.
    */
   const value_type& value() const
   { return m_value; }

   accumulator_value_type& value()
   { return m_value; }

   /**
    * Returns the name of the operator implemented by the accumulator.
    */
   static std::string name()
   { return OP<Real>::name(); }

private:
   accumulator_value_type m_value;
};

}

#endif
