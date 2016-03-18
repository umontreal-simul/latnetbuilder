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

#ifndef LATBUILDER__COMPRESSED_SUM_H
#define LATBUILDER__COMPRESSED_SUM_H

#include "latbuilder/Storage.h"

namespace LatBuilder {

/**
 * Sum of all the elements of a (possibly compressed) vector.
 *
 * In an compressed vector some elements are implicitly repeated.
 */
template <Compress COMPRESS, class E>
typename Storage<LatType::ORDINARY, COMPRESS>::MeritValue
compressedSum(
      const Storage<LatType::ORDINARY, COMPRESS>& storage, 
      const boost::numeric::ublas::vector_expression<E>& e
      ) 
{
   const auto& vec = e();

   Real sum = 0.0;
   for (const auto& x : vec)
      sum += x;

   if (COMPRESS == Compress::SYMMETRIC) {
      // compression ratio except first element
      sum *= 2;
      sum -= vec(0);
      // if even number of points, last element is not repeated
      if (storage.sizeParam().numPoints() % 2 == 0)
         sum -= vec(vec.size() - 1);
   }

   return sum;
}


/**
 * Returns the per-level sums of the elements of the compressed multilevel
 * vector expression \c e.
 */
template <Compress COMPRESS, class E>
typename Storage<LatType::EMBEDDED, COMPRESS>::MeritValue
compressedSum(
      const Storage<LatType::EMBEDDED, COMPRESS>& storage, 
      const boost::numeric::ublas::vector_expression<E>& e
      ) 
{
   const auto ranges = storage.levelRanges();

   RealVector out(ranges.size());
   Real cumulative = 0.0;

   auto itOut = out.begin();

   for (const auto& range : ranges) {

      boost::numeric::ublas::vector_range<const E> subvec(e(), range);

      double sum = 0.0;
      for (const auto& x : subvec)
         sum += x;

      auto level = itOut - out.begin();

      if (COMPRESS == Compress::SYMMETRIC) {
         if (level >= (storage.sizeParam().base() == 2 ? 2 : 1))
            // compression ratio except if uncompressed level has only one element
            sum *= 2;
      }

      *itOut = cumulative += sum;

      ++itOut;
   }
   
   return out;
}

}

#endif
