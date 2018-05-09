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

#ifndef LATBUILDER__KERNEL__FUNCTOR_ADAPTOR_H
#define LATBUILDER__KERNEL__FUNCTOR_ADAPTOR_H

#include "latbuilder/Kernel/Base.h"

#include <stdexcept>

namespace LatBuilder { namespace Kernel {

/**
 * Generic kernel for functors.
 *
 * This class allows for polymorphism while inlining the functor calls in the
 * loop that initializes new vectors.
 *
 * \tparam FUNCTOR      Type of functor.
 */
template <typename FUNCTOR>
class FunctorAdaptor : public Base<FunctorAdaptor<FUNCTOR>> {
public:
   typedef FUNCTOR Functor;

   /**
    * Constructor.
    */
   FunctorAdaptor(Functor functor = Functor()):
      m_functor(std::move(functor))
   {}

   /**
    * Creates a new vector of kernel values.
    *
    * The values of the kernel evaluated at \c sizeParam.numPoints() regular
    * intervals in \f$[0,1)\f$ are stored in a linear vector.
    * The intervals are of size \c 1/sizeParam.numPoints() and the first point is
    * at 0.
    *
    * \return The newly created vector.
    *
    * The \f$i^{\text{th}}\f$ element  \f$\omega_i\f$ is:
    *- \f$\omega(i/n)\f$ in the case of an ordinary lattice with modulus \f$n\f$.
    *-  \f$\omega((\nu_m(\frac{i(z)}{P(z)}))\f$ in the case of a polynomial lattice of modulus \f$P(z)\f$ (\f$ i(z) = \sum a_iz^i\f$ where \f$i =\sum a_i2^i\f$).
    *
    *
    * \remark Checks that the functor and the compression are compatible, or
    * throws a std::logic_error.
    */
   template <LatticeType LR, PointSetType L, Compress C, PerLevelOrder P >
   RealVector valuesVector(
         const Storage<LR, L, C, P>& storage
         ) const
   { 
      if (storage.symmetric() and not m_functor.symmetric())
        throw std::logic_error("functor must be symmetric in order to use symmetric compression");

      const auto numPoints = storage.virtualSize();
      const auto modulus = storage.sizeParam().modulus();

      RealVector vec(storage.size());
      auto proxy = storage.unpermuted(vec);

      for (size_t i = 0; i < vec.size(); i++)
         proxy(i) = m_functor(Real(LatticeTraits<LR>::ToKernelIndex(i,modulus)) / numPoints, modulus);

      return vec;
   }

   /**
    * Returns \c true if the kernel takes the same value at points \f$x\f$ and
    * \f$1 - x\f$ for \f$x \in [0,1)\f$.
    */
   bool symmetric() const
   { return m_functor.symmetric(); }

   static constexpr Compress suggestedCompression()
   { return Functor::suggestedCompression(); }

   /**
    * Returns the name of the kernel.
    */
   std::string name() const
   { return m_functor.name(); }

protected:
   const Functor& functor() const
   { return m_functor; }

private:
   Functor m_functor;
};

}}

#endif
