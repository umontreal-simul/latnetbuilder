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

#include "latbuilder/MeritSeq/ConcreteCoordUniformState-P.h"

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// ProductWeights
//========================================================================

template <LatType LAT, Compress COMPRESS>
void
ConcreteCoordUniformState<LAT, COMPRESS, LatCommon::ProductWeights>::
reset()
{
   CoordUniformState<LAT, COMPRESS>::reset();
   m_state.clear();
   m_state = boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 1.0);
}

//===========================================================================

template <LatType LAT, Compress COMPRESS>
void
ConcreteCoordUniformState<LAT, COMPRESS, LatCommon::ProductWeights>::
update(const RealVector& kernelValues, Modulus gen)
{
   CoordUniformState<LAT, COMPRESS>::update(kernelValues, gen);

   auto stridedKernelValues = this->storage().strided(kernelValues, gen);

   const auto newCoordinate = this->dimension() - 1;

   const Real weight = m_weights.getWeightForCoordinate(newCoordinate);

   const auto ones = boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 1.0);

   m_state = boost::numeric::ublas::element_prod(
         ones + weight * stridedKernelValues,
         m_state
         );
}

//===========================================================================

template <LatType LAT, Compress COMPRESS>
RealVector
ConcreteCoordUniformState<LAT, COMPRESS, LatCommon::ProductWeights>::
weightedState() const
{
   const auto nextCoordinate = this->dimension();

   const Real weight = m_weights.getWeightForCoordinate(nextCoordinate);

   return weight * m_state;
}

template class ConcreteCoordUniformState<LatType::ORDINARY, Compress::NONE,      LatCommon::ProductWeights>;
template class ConcreteCoordUniformState<LatType::ORDINARY, Compress::SYMMETRIC, LatCommon::ProductWeights>;
template class ConcreteCoordUniformState<LatType::EMBEDDED, Compress::NONE,      LatCommon::ProductWeights>;
template class ConcreteCoordUniformState<LatType::EMBEDDED, Compress::SYMMETRIC, LatCommon::ProductWeights>;

}}
