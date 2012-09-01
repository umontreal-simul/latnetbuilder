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

#ifndef LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_POD_H
#define LATBUILDER__MERIT_SEQ__CONCRETE_COORD_SYM_STATE_POD_H

#include "latbuilder/MeritSeq/CoordSymState.h"
#include "latbuilder/Storage.h"

#include "latcommon/PODWeights.h"

#include <vector>

namespace LatBuilder { namespace MeritSeq {

// forward declaration
template <LatType LAT, Compress COMPRESS, class WEIGHTS> class ConcreteCoordSymState;

/**
 * Implementation of CoordSymState for POD weights.
 */
template <LatType LAT, Compress COMPRESS>
class ConcreteCoordSymState<LAT, COMPRESS, LatCommon::PODWeights> :
   public CoordSymState<LAT, COMPRESS> {
public:
   /**
    * Constructor.
    * 
    * Sets \f$\mathcal D_0^2 = 0\f$ and \f$\boldsymbol p_{0,\ell} =
    * \boldsymbol 1\f$.
    *
    * \param storage       Storage configuration.
    * \param weights       POD weights
    */
   ConcreteCoordSymState(
         const Storage<LAT, COMPRESS>& storage,
         const LatCommon::PODWeights& weights
         ):
      CoordSymState<LAT, COMPRESS>(storage),
      m_weights(weights)
   { reset(); }

   void reset();

   /**
    * \copydoc CoordSymState::update()
    *
    * Computes
    * \f[
    *    \boldsymbol p_{s,\ell} =
    *       \boldsymbol p_{s-1,\ell} + \boldsymbol\omega_s \odot \boldsymbol p_{s-1,\ell-1}.
    * \f]
    */
   void update(const RealVector& kernelValues, Modulus gen);

   /**
    * \copydoc CoordSymState::weightedState()
    *
    * Computes
    * \f[
    *    \boldsymbol q_s = \sum_{\ell=0}^s \Gamma_{\ell+1} \boldsymbol p_{s,\ell}.
    * \f]
    */
   RealVector weightedState() const;

   /// \copydoc CoordSymState::clone()
   std::unique_ptr<CoordSymState<LAT, COMPRESS>> clone() const
   { return std::unique_ptr<CoordSymState<LAT, COMPRESS>>(new ConcreteCoordSymState(*this)); }

private:
   const LatCommon::PODWeights& m_weights;

   // m_state[level](i)
   std::vector<RealVector> m_state;
};

extern template class ConcreteCoordSymState<LatType::ORDINARY, Compress::NONE,      LatCommon::PODWeights>;
extern template class ConcreteCoordSymState<LatType::ORDINARY, Compress::SYMMETRIC, LatCommon::PODWeights>;
extern template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::NONE,      LatCommon::PODWeights>;
extern template class ConcreteCoordSymState<LatType::EMBEDDED, Compress::SYMMETRIC, LatCommon::PODWeights>;

}}

#endif
