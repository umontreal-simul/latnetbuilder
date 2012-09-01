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
    * \remark Checks that the functor and the compression are compatible, or
    * throws a std::logic_error.
    */
   template <LatType L, Compress C>
   RealVector valuesVector(
         const Storage<L, C>& storage
         ) const
   { 
      if (storage.symmetric() and not m_functor.symmetric())
        throw std::logic_error("functor must be symmetric in order to use symmetric compression");

      const auto modulus = storage.virtualSize();

      RealVector vec(storage.size());
      auto proxy = storage.unpermuted(vec);

      for (size_t i = 0; i < vec.size(); i++)
         proxy(i) = m_functor(Real(i) / modulus, modulus);

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
