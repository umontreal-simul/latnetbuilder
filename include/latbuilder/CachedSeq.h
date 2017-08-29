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
template <class BASE, LatticeType LR, LatEmbed LAT, Compress COMPRESS>
class CachedSeq : public BASE {
public:
   typedef BASE Base;
   typedef LatBuilder::Storage<LR, LAT, COMPRESS> Storage;
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
