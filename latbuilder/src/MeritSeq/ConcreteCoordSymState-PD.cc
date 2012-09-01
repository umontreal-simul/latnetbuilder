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

#include "latbuilder/MeritSeq/ConcreteCoordSymState-PD.h"

namespace LatBuilder { namespace MeritSeq {

//========================================================================
// ProjectionDependentWeights
//========================================================================

template <LatType LAT, Compress COMPRESS>
void
ConcreteCoordSymState<LAT, COMPRESS, LatCommon::ProjectionDependentWeights>::
reset()
{
   CoordSymState<LAT, COMPRESS>::reset();
   m_state.clear();
   // empty set
   m_state[LatCommon::Coordinates()] =
      boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 1.0);
   m_gen.clear();
}

//===========================================================================

template <LatType LAT, Compress COMPRESS>
const RealVector&
ConcreteCoordSymState<LAT, COMPRESS, LatCommon::ProjectionDependentWeights>::
createStateVector(const LatCommon::Coordinates& projection, const RealVector& kernelValues)
{
   auto it = m_state.find(projection);
   if (it != m_state.end())
      return it->second;

   // select largest index
   const LatCommon::Coordinates::value_type largestCoord = *projection.rbegin();

   // remove largest coordinate index
   LatCommon::Coordinates baseProjection = projection;
   baseProjection.erase(largestCoord);
   // create base state vector
   const RealVector& baseState = createStateVector(baseProjection, kernelValues);

   // compute merit value for new projection
   auto stridedKernelValues = this->storage().strided(
         kernelValues,
         m_gen[largestCoord]
         );
   return m_state[projection] = boost::numeric::ublas::element_prod(
         stridedKernelValues,
         baseState
         );
}

//===========================================================================

template <LatType LAT, Compress COMPRESS>
void
ConcreteCoordSymState<LAT, COMPRESS, LatCommon::ProjectionDependentWeights>::
update(const RealVector& kernelValues, Modulus gen)
{
   CoordSymState<LAT, COMPRESS>::update(kernelValues, gen);
   m_gen.push_back(gen);

   // Create a new state vector for each projection $\mathfrak u$ such that
   // $\max \mathfrak u$ < $j$ and that $\mathfrak u \cup \{j\}$ has a nonzero
   // weight.
   const auto nextCoordinate = this->dimension();
   for (const auto& pw : m_weights.getWeightsForLargestIndex(nextCoordinate)) {
      // remove largest coordinate index
      LatCommon::Coordinates proj = pw.first;
      proj.erase(*proj.rbegin());
      // create state vector
      createStateVector(proj, kernelValues);
   }
}

//===========================================================================

template <LatType LAT, Compress COMPRESS>
RealVector
ConcreteCoordSymState<LAT, COMPRESS, LatCommon::ProjectionDependentWeights>::
weightedState() const
{
   using LatCommon::Coordinates;

   const auto nextCoordinate = this->dimension();

   RealVector weightedState =
      boost::numeric::ublas::scalar_vector<Real>(this->storage().size(), 0.0);

   for (const auto& pw : m_weights.getWeightsForLargestIndex(nextCoordinate)) {
      // remove largest coordinate index
      LatCommon::Coordinates proj = pw.first;
      proj.erase(*proj.rbegin());
      // find state vector
      auto it = m_state.find(proj);
      if (it == m_state.end())
         throw std::runtime_error("projection-dependent state was not created");
      // contribute to weighted state
      weightedState += pw.second * it->second;
   }

   return weightedState;
}

//===========================================================================

template class ConcreteCoordSymState<LatType::ORDINARY, Compress::NONE,      LatCommon::ProjectionDependentWeights>;
template class ConcreteCoordSymState<LatType::ORDINARY, Compress::SYMMETRIC, LatCommon::ProjectionDependentWeights>;
template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::NONE,      LatCommon::ProjectionDependentWeights>;
template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::SYMMETRIC, LatCommon::ProjectionDependentWeights>;

}}
