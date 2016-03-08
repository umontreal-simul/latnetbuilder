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

#ifndef LATBUILDER__CACHED_SEQUENCE
#define LATBUILDER__CACHED_SEQUENCE

#include "latbuilder/Storage.h"

#include <vector>

namespace LatBuilder {

/**
 * Cached sequence wrapper.
 *
 * Allows the elements of a dynamically generated sequence to be pre-computed
 * and stored in a container.
 *
 * Example:
 * \code
 * CachedSeq<MySeq> seq;
 * seq = MySeq(...);
 * for (const auto& elem : seq)
 *   std::cout << elem << std::endl;
 * \endcode
 */
template <class BASE, LatType LAT, Compress COMPRESS>
class CachedSeq : public BASE {
public:
   typedef BASE Base;
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename Storage::value_type value_type;
   typedef typename Storage::size_type size_type;
   typedef typename Storage::const_iterator const_iterator;

   CachedSeq():
      Base()
   {}

   CachedSeq(Base base):
      Base(std::move(base))
   { prepareCache(); }

   CachedSeq& operator=(Base base) {
      Base::operator=(std::move(base));
      prepareCache();
      return *this;
   }

   size_type size() const {
      return Storage::size();
   }

   const_iterator begin() const {
      return Storage::begin();
   }

   const_iterator end() const {
      return Storage::end();
   }

private:
   void prepareCache() {
      Storage::assign(Base::begin(), Base::end());
   }
};

}

#endif
