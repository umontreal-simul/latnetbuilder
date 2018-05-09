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

#include "latbuilder/Storage.h"
#include "latbuilder/TextStream.h"

using namespace LatBuilder;
using TextStream::operator<<;

template <LatType L, Compress C>
void test(const Storage<L, C>& storage)
{
   std::cout << "==> storage / compression: " << Storage<L, C>::name() << std::endl;
   std::cout << "    virtual / actual sizes: " << storage.virtualSize() << " / " << storage.size() << std::endl;

   RealVector original(storage.size());

   auto unpermuted = storage.unpermuted(original);

   for (RealVector::size_type j = unpermuted.size(); j > 0; j--)
      unpermuted[j - 1] = j - 1;

   auto strided = storage.strided(original, 3);

   std::cout << "    original:   " << original << std::endl;
   std::cout << "    unpermuted: " << unpermuted << std::endl;
   std::cout << "    strided(3): " << strided << std::endl;
}

int main(int argc, const char *argv[])
{
   test(Storage<LatType::ORDINARY, Compress::NONE     >(12));
   test(Storage<LatType::ORDINARY, Compress::SYMMETRIC>(12));
   test(Storage<LatType::EMBEDDED, Compress::NONE     >(16));
   test(Storage<LatType::EMBEDDED, Compress::SYMMETRIC>(16));

   return 0;
}
