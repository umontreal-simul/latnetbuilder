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
 * Accumulator template.
 *
 * \tparam OP              Scalar binary operator.
 *                         For example, Functor::Sum or Functor::Max.
 */
template <template <typename> class OP, typename VAL>
class Accumulator {
public:

   Accumulator(VAL initialValue):
      m_value(initialValue)
   {}

   /**
    * Feeds a new value to the accumulator.
    */
   void accumulate(const VAL& value)
   { accumulate(1.0, value); }

   /**
    * Feeds \c value multiplied by \c weight to the accumulator.
    * \c weight is assumed to already be raised at the power \c power.
    */
   void accumulate(Real weight, const VAL& value, Real power = 1.0)
   { m_value = Vectorize::apply<OP<Real>>(m_value, weight * Vectorize::apply<Functor::Pow>(value, power)); }

   /**
    * Returns the current value of the accumulator.
    */
   const VAL& value() const
   { return m_value; }

   VAL& value()
   { return m_value; }

   /**
    * Returns the name of the operator implemented by the accumulator.
    */
   static std::string name()
   { return OP<Real>::name(); }

private:
   VAL m_value;
};

}

#endif
