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

#ifndef LATBUILDER__COMPRESS_TRAITS_H
#define LATBUILDER__COMPRESS_TRAITS_H

#include "latbuilder/Types.h"

namespace LatBuilder {

/**
 * Compression types for vectors and matrices.
 *
 * \see CompressTraits<Compress::NONE> CompressTraits<Compress::SYMMETRIC>
 */
template <Compress COMPRESS>
struct CompressTraits;

/**
 * No compression.
 *
 * Without compression, the virtual vector is identical to the stored vector.
 * No index conversion is necessary.
 */
template<>
struct CompressTraits<Compress::NONE> {
   static constexpr bool symmetric() { return false; }
   /// Returns \c n.
   static size_t size(size_t n) { return n; }
   /// Returns \c i.
   static size_t compressIndex(size_t i, size_t n) { return i; }
   /// Returns "none".
   static constexpr const char* name() { return "none"; }
   /// Returns 1.
   static int indexCompressionRatio(size_t i, size_t n)
   { return 1; }
   /// Returns 1.
   static int levelCompressionRatio(Modulus base, Level level)
   { return 1; }
};

/**
 * Symmetric compression.
 * 
 * With symmetric compression, the second half of a virtual vector is identical
 * to the first half, but in reverse order.  Thus, only the first half is
 * stored in memory, and when an component from the second half is accessed, it
 * is mapped to the identical element from the first half.  Modifying one
 * modifies the other.
 */
template<>
struct CompressTraits<Compress::SYMMETRIC> {
   static constexpr bool symmetric() { return true; }
   /**
    * Returns the internal vector size corresponding to natural size \c n.
    * Natural points are at \f$ i / n \f$ for \f$i = 0, \dots, n-1\f$.
    * All points up to the middle one, inclusively, contain unique information.
    * The middle point is found at \f$ (n + 1) / 2 \f$ if \f$ n \f$ is odd, and
    * at \f$ n / 2 + 1 \f$ if \f$ n \f$ is even.  This gives the necessary
    * storage size to contain the complete information.
    */
   static size_t size(size_t n) { return n == 0 ? 0 : n / 2 + 1; }
   /**
    * Returns the index at which the \c i -th natural element is stored.
    * This is \f$ \min(i, n - i) \f$.
    */
   static size_t compressIndex(size_t i, size_t n) { return std::min(i, n - i); }
   /// Returns "symmetric".
   static constexpr const char* name() { return "symmetric"; }

   /**
    * Returns the compression ratio of the element at index \c i for vector size
    * \c n.
    *
    * The first element of a vector is never compressed.
    * The last element of a vector is not compressed when \c n is even and is
    * compressed by a factor of 2 otherwise.
    * The rest of the elements are compressed by a factor of 2.
    */
   static int indexCompressionRatio(size_t i, size_t n)
   { return i == 0 or (i == n - 1 and n % 2 == 0) ? 1 : 2; }

   /**
    * Returns the compression ratio for the elements on level \c level in base
    * \c base.
    *
    * If \c base is 2, levels 0 and 1 are not compressed, and higher levels are
    * compressed by a factor of 2.
    *
    * If \c base is larger than 2, level 0 is not compressed, and higher levels
    * are compressed by a factor of 2.
    */
   int levelCompressionRatio(Modulus base, Level level) const
   { return level >= (base == 2 ? 2 : 1) ? 2 : 1; }
};

}

#endif
