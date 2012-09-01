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

#include "latbuilder/MeritSeq/ConcreteCoordSymState-P.h"

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// ProductWeights
//========================================================================

template <LatType LAT, Compress COMPRESS>
void
ConcreteCoordSymState<LAT, COMPRESS, LatCommon::ProductWeights>::
reset()
{
   CoordSymState<LAT, COMPRESS>::reset();
   m_state.clear();
   m_state = boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 1.0);
}

//===========================================================================

template <LatType LAT, Compress COMPRESS>
void
ConcreteCoordSymState<LAT, COMPRESS, LatCommon::ProductWeights>::
update(const RealVector& kernelValues, Modulus gen)
{
   CoordSymState<LAT, COMPRESS>::update(kernelValues, gen);

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
ConcreteCoordSymState<LAT, COMPRESS, LatCommon::ProductWeights>::
weightedState() const
{
   const auto nextCoordinate = this->dimension();

   const Real weight = m_weights.getWeightForCoordinate(nextCoordinate);

   return weight * m_state;
}

template class ConcreteCoordSymState<LatType::ORDINARY, Compress::NONE,      LatCommon::ProductWeights>;
template class ConcreteCoordSymState<LatType::ORDINARY, Compress::SYMMETRIC, LatCommon::ProductWeights>;
template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::NONE,      LatCommon::ProductWeights>;
template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::SYMMETRIC, LatCommon::ProductWeights>;

}}
