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

#ifndef LATBUILDER__STORAGE_H
#define LATBUILDER__STORAGE_H

#include "latbuilder/Types.h"
#include "latbuilder/IndexMap.h"

#include <boost/numeric/ublas/vector_proxy.hpp>

#include <string>

namespace LatBuilder {

/**
 * Storage policy.
 */
template <LatType, Compress> class Storage;

/**
 * Storage traits.
 */
template <class DERIVED>
struct StorageTraits;

/**
 * Base class for storage types.
 *
 * \tparam DERIVED   Concrete storage type.
 */
template <class DERIVED>
class BasicStorage {
protected:
   BasicStorage(const BasicStorage&) = default;

protected:
   typedef typename StorageTraits<DERIVED>::Compress  Compress;

public:

   typedef typename StorageTraits<DERIVED>::size_type size_type;
   typedef typename StorageTraits<DERIVED>::MeritValue MeritValue;
   typedef typename StorageTraits<DERIVED>::SizeParam SizeParam;
   typedef typename StorageTraits<DERIVED>::Unpermute Unpermute;
   typedef typename StorageTraits<DERIVED>::Stride    Stride;

   BasicStorage(SizeParam sizeParam):
      m_sizeParam(std::move(sizeParam))
   {}

   static std::string name()
   { return DERIVED::shortname() + " / " + Compress::name(); }

   size_type virtualSize() const
   { return derived().virtualSize(); }

   size_type size() const
   { return Compress::size(virtualSize()); }

   const SizeParam& sizeParam() const
   { return m_sizeParam; }

   bool symmetric() const
   { return Compress::symmetric(); }

   bool operator==(const BasicStorage& other) const
   { return sizeParam() == other.sizeParam(); }

   bool operator!=(const BasicStorage& other) const
   { return !operator==(other); }

   MeritValue createMeritValue(Real value) const
   { return derived().createMeritValue(value); }

   /**
    * Returns a vector proxy to access the vector's elements in their natural
    * order.
    *
    * \todo Code example.
    */
   template <class V>
   boost::numeric::ublas::vector_indirect<V, IndexMap<Unpermute>> unpermuted(
         boost::numeric::ublas::vector_container<V>& vec
         ) const
   {
      return boost::numeric::ublas::vector_indirect<V, IndexMap<Unpermute>>(
            vec(),
            IndexMap<Unpermute>(Unpermute(derived()))
            );
   }

   /**
    * Returns a vector proxy to access the vector's elements with a periodic
    * jump of \c stride across the elements.
    */
   template <class V>
   boost::numeric::ublas::vector_indirect<const V, IndexMap<Stride>> strided(
         const boost::numeric::ublas::vector_container<V>& vec,
         size_type stride
         ) const
   {
      return boost::numeric::ublas::vector_indirect<const V, IndexMap<Stride>>(
            vec(),
            IndexMap<Stride>(Stride(derived(), stride))
            );
   }

private:
   SizeParam m_sizeParam;

   DERIVED& derived()
   { return static_cast<DERIVED&>(*this); }

   const DERIVED& derived() const
   { return static_cast<const DERIVED&>(*this); }
};

}

#include "latbuilder/Storage-ORDINARY.h"
#include "latbuilder/Storage-EMBEDDED.h"

#endif
