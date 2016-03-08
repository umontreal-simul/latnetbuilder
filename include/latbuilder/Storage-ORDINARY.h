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

#ifndef LATBUILDER__STORAGE_ORDINARY_H
#define LATBUILDER__STORAGE_ORDINARY_H

#include "latbuilder/Storage.h"
#include "latbuilder/CompressTraits.h"
#include "latbuilder/SizeParam.h"

namespace LatBuilder {

template <Compress COMPRESS>
struct StorageTraits<Storage<LatType::ORDINARY, COMPRESS>> {
   typedef Modulus            size_type;
   typedef CompressTraits<COMPRESS> Compress;
   typedef Real               MeritValue;
   typedef LatBuilder::SizeParam<LatType::ORDINARY> SizeParam;

   /**
    * Unpermuted permutation.
    */
   class Unpermute {
   public:
      typedef StorageTraits::size_type size_type;

      Unpermute(Storage<LatType::ORDINARY, COMPRESS> storage): m_storage(std::move(storage)) {}
      size_type operator() (size_type i) const { return Compress::compressIndex(i, m_storage.virtualSize()); }
      size_type size() const { return m_storage.virtualSize(); }
   private:
      Storage<LatType::ORDINARY, COMPRESS> m_storage;
   };

   /**
    * Stride permutation.
    *
    * The elements are visited by jumping across a certain number
    * of elements (the stride length) periodically.
    *
    * Consider the unpermuted vector \f$(v_1, \dots, v_n)\f$ for some positive
    * integer \f$n\f$ and a positive integer \f$a\f$, the stride length.
    * Then, the \f$j\f$-th component of the permuted vector has value \f$v_{j
    * a \bmod n}\f$.
    */
   class Stride {
   public:
      typedef StorageTraits::size_type size_type;

      Stride(Storage<LatType::ORDINARY, COMPRESS> storage, size_type stride):
         m_storage(std::move(storage)),
         m_stride(stride)
      {}

      size_type operator() (size_type i) const
      {
         const auto numPoints = m_storage.sizeParam().numPoints();
         return Compress::compressIndex(m_stride * i % numPoints, numPoints);
      }

      size_type size() const
      { return m_storage.size(); }

   private:
      Storage<LatType::ORDINARY, COMPRESS> m_storage;
      size_type m_stride;
   };

};

/**
 * Storage class for flat vectors.
 *
 * The vector elements are not permuted but compression is applied to vector
 * indices.
 *
 * \tparam COMPRESS     Compression type (either None or Symmetric).
 */
template <Compress COMPRESS>
class Storage<LatType::ORDINARY, COMPRESS> :
   public BasicStorage<Storage<LatType::ORDINARY, COMPRESS>> {

   typedef Storage<LatType::ORDINARY, COMPRESS> self_type;

public:

   typedef typename self_type::size_type  size_type;
   typedef typename self_type::MeritValue MeritValue;
   typedef typename self_type::SizeParam  SizeParam;

   static std::string shortname()
   { return "flat storage"; }

   Storage(SizeParam sizeParam):
      BasicStorage<Storage>(std::move(sizeParam))
   {}

   size_type virtualSize() const
   { return this->sizeParam().numPoints(); }

   MeritValue createMeritValue(Real value = 0.0) const
   { return value; }
};

}

#endif
