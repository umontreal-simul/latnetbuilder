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

#include "latbuilder/MeritSeq/ConcreteCoordSymState-OD.h"

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// OrderDependentWeights
//========================================================================

template <LatType LAT, Compress COMPRESS>
void
ConcreteCoordSymState<LAT, COMPRESS, LatCommon::OrderDependentWeights>::
reset()
{
   CoordSymState<LAT, COMPRESS>::reset();
   m_state.clear();
   // order 0
   m_state.push_back(RealVector(this->storage().size(), 1.0));
}

//===========================================================================

template <LatType LAT, Compress COMPRESS>
void
ConcreteCoordSymState<LAT, COMPRESS, LatCommon::OrderDependentWeights>::
update(const RealVector& kernelValues, Modulus gen)
{
   CoordSymState<LAT, COMPRESS>::update(kernelValues, gen);

   auto stridedKernelValues = this->storage().strided(kernelValues, gen);

   // add new order
   m_state.push_back(RealVector(this->storage().size(), 0.0));

   // recursive update by decreasing order to avoid unwanted overwriting
   for (size_t order = m_state.size() - 1; order > 0; order--)
      m_state[order] += boost::numeric::ublas::element_prod(
            stridedKernelValues,
            m_state[order - 1]
            );
}

//===========================================================================

template <LatType LAT, Compress COMPRESS>
RealVector
ConcreteCoordSymState<LAT, COMPRESS, LatCommon::OrderDependentWeights>::
weightedState() const
{
   using LatCommon::Coordinates;

   RealVector weightedState =
      boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 0.0);

   for (Coordinates::size_type order = 0; order < m_state.size(); order++) {

      Real weight = m_weights.getWeightForOrder(order + 1);

      if (weight == 0.0)
         continue;

      weightedState += weight * m_state[order];
   }

   return weightedState;
}

template class ConcreteCoordSymState<LatType::ORDINARY, Compress::NONE,      LatCommon::OrderDependentWeights>;
template class ConcreteCoordSymState<LatType::ORDINARY, Compress::SYMMETRIC, LatCommon::OrderDependentWeights>;
template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::NONE,      LatCommon::OrderDependentWeights>;
template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::SYMMETRIC, LatCommon::OrderDependentWeights>;

}}
