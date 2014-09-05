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

#include "latbuilder/Storage.h"
#include "latbuilder/TextStream.h"

using namespace LatBuilder;
using TextStream::operator<<;

template <LatType L, Compress C>
void test(const Storage<L, C>& storage)
{
   std::cout << "storage name: " << Storage<L, C>::name() << std::endl;
   std::cout << "  virtual size: " << storage.virtualSize() << std::endl;

   RealVector original(storage.size());

   auto unpermuted = storage.unpermuted(original);

   for (RealVector::size_type j = unpermuted.size(); j > 0; j--)
      unpermuted[j - 1] = j - 1;

   auto strided = storage.strided(original, 3);

   std::cout << "  original:   " << original << std::endl;
   std::cout << "  unpermuted: " << unpermuted << std::endl;
   std::cout << "  strided(3): " << strided << std::endl;
}

int main(int argc, const char *argv[])
{
   test(Storage<LatType::ORDINARY, Compress::NONE     >(12));
   test(Storage<LatType::ORDINARY, Compress::SYMMETRIC>(12));
   test(Storage<LatType::EMBEDDED, Compress::NONE     >(16));
   test(Storage<LatType::EMBEDDED, Compress::SYMMETRIC>(16));

   return 0;
}
