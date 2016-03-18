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

#ifndef LATBUILDER__STORAGE_H
#define LATBUILDER__STORAGE_H

#include "latbuilder/Types.h"
#include "latbuilder/IndexMap.h"

#include <boost/numeric/ublas/vector_proxy.hpp>

#include <string>

namespace LatBuilder {

/**
 * Storage policy.
 *
 * A storage policy indicates how kernel values are stored and represented in a
 * vector.
 * This template class acts as a selector for specialized templates, based on
 * the type of lattice and on the type of compression.
 *
 * The output from \ref Storage.cc gives an intuition of how this works for
 * different combination of lattice type and of compression.
 */
template <LatType, Compress> class Storage;

/**
 * Storage traits.
 *
 * Specializations of this class template must define the types:
 * - size_type: size type for vectors;
 * - Compress: a specialization of CompressTraits;
 * - MeritValue: data type for merit values;
 * - SizeParam: specilization of LatBuilder::SizeParam;
 * - Unpermute: operator that maps a logical vector index to a storage index;
 * - Stride: operator that maps a point index to a storage index (a logical
 *   vector index is first obtained by multiplying the point index is
 *   multiplied with the <em>stride</em> factor, an integer value which
 *   normally corresponds to that of a generating vector component.
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

/**
 * \example Storage.cc
 * This examples shows how to use the LatBuilder::Storage template class.
 *
 * The output is as follows:
 * \code{.unparsed}
 * ==> storage / compression: flat storage / none
 *     virtual / actual sizes: 12 / 12
 *     original:   [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
 *     unpermuted: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
 *     strided(3): [0, 3, 6, 9, 0, 3, 6, 9, 0, 3, 6, 9]
 * ==> storage / compression: flat storage / symmetric
 *     virtual / actual sizes: 12 / 7
 *     original:   [0, 1, 2, 3, 4, 5, 6]
 *     unpermuted: [0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1]
 *     strided(3): [0, 3, 6, 3, 0, 3, 6]
 * ==> storage / compression: multilevel storage / none
 *     virtual / actual sizes: 16 / 16
 *     original:   [0, 8, 4, 12, 2, 10, 14, 6, 1, 13, 9, 5, 15, 3, 7, 11]
 *     unpermuted: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
 *     strided(3): [0, 8, 12, 4, 6, 14, 10, 2, 3, 7, 11, 15, 13, 9, 5, 1]
 * ==> storage / compression: multilevel storage / symmetric
 *     virtual / actual sizes: 16 / 9
 *     original:   [0, 8, 4, 2, 6, 1, 3, 7, 5]
 *     unpermuted: [0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1]
 *     strided(3): [0, 8, 4, 6, 2, 3, 7, 5, 1]
 * \endcode
 */
}

#include "latbuilder/Storage-ORDINARY.h"
#include "latbuilder/Storage-EMBEDDED.h"

#endif
