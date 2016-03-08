// Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
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

#ifndef LATBUILDER__MERIT_SEQ__COORD_SYM_STATE_H
#define LATBUILDER__MERIT_SEQ__COORD_SYM_STATE_H

#include "latbuilder/Types.h"
#include "latbuilder/Storage.h"

#include <memory>

namespace LatBuilder { namespace MeritSeq {

/**
 * Base base class for states used in the evaluation coordinate-uniform
 * figures of merit.
 *
 * The complete state is stored internally and can be updated with update().
 * The weighted state can be obtained with #weightedState().
 *
 * \sa CoordUniformEval
 *
 * \note By taking a RealVector as a parameter, calls to update() force a copy
 * from some boost::numeric::ublas::vector_expression<E> to a new instance
 * of RealVector.  This is the cost for making update() a virtual function: it
 * cannot be a template.
 */
template <LatType LAT, Compress COMPRESS>
class CoordUniformState {
public:
   CoordUniformState(Storage<LAT, COMPRESS> storage):
      m_storage(std::move(storage)),
      m_dimension(0)
   {}

   virtual ~CoordUniformState()
   {}

   /**
    * Resets the state to dimension 0 with new specified storage configuration.
    */
   virtual void reset()
   { m_dimension = 0; }

   /**
    * Updates the current state using the specified row of the permuted matrix
    * of kernel values.
    *
    * This corresponds to appending a component \f$a_j\f$ to the generating
    * vector \f$\boldsymbol a = (a_1, \dots, a_{j-1})\f$.
    * To each possible value of \f$a_j\f$ corresponds a distinct row of the
    * matrix \f$\boldsymbol\Omega\f$ of kernel values.
    *
    * This increases the internal dimension counter.
    */
   virtual void update(const RealVector& kernelValues, Modulus gen)
   { m_dimension++; }

   /**
    * Computes and returns the weighted state vector \f$\boldsymbol q_s\f$.
    */
   virtual RealVector weightedState() const = 0;

   /**
    * Returns a pointer to the storage configuration.
    */
   const Storage<LAT, COMPRESS>& storage() const
   { return m_storage; }

   /**
    * Returns the value of the internal dimension counter.
    */
   Dimension dimension() const
   { return m_dimension; }

   /**
    * Returns a copy of this instance.
    */
   virtual std::unique_ptr<CoordUniformState> clone() const = 0;

private:
   Storage<LAT, COMPRESS> m_storage;
   Dimension m_dimension;
};

}}

#endif
