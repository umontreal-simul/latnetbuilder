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
