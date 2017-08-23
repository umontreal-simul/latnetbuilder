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
template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO >
class CoordUniformState {
public:

  

   CoordUniformState(Storage<LR, LAT, COMPRESS, PLO> storage):
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
   virtual void update(const RealVector& kernelValues, typename LatticeTraits<LR>::GenValue gen)
   { m_dimension++; }

   /**
    * Computes and returns the weighted state vector \f$\boldsymbol q_s\f$.
    */
   virtual RealVector weightedState() const = 0;

   /**
    * Returns a pointer to the storage configuration.
    */
   const Storage<LR, LAT, COMPRESS, PLO>& storage() const
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
   Storage<LR, LAT, COMPRESS, PLO> m_storage;
   Dimension m_dimension;
};

}}

#endif
