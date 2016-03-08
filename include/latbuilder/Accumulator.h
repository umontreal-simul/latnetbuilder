// Copyright (c) 2012, 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
// 
// This file is part of Lattice Builder.
// 
// Lattice Builder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Lattice Builder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

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
