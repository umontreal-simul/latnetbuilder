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
#include "latbuilder/SizeParam.h"

#include <iostream>

using namespace LatBuilder;

//! [all]
template <LatType LAT, Compress COMP>
void test(Modulus n)
{
   SizeParam<LAT> size(n);
   Storage<LAT, COMP> storage(size);
   std::cout << "storage name: " << storage.name() << std::endl;
   std::cout << "  size parameter: " << storage.sizeParam() << std::endl;
   std::cout << "  virtual size:   " << storage.virtualSize() << std::endl;
   std::cout << "  actual size:    " << storage.size() << std::endl;
}

int main()
{

   Modulus n = 16;
   test<LatType::ORDINARY, Compress::NONE>(n);
   test<LatType::EMBEDDED, Compress::NONE>(n);
   test<LatType::ORDINARY, Compress::SYMMETRIC>(n);
   test<LatType::EMBEDDED, Compress::SYMMETRIC>(n);

   return 0;
}
//! [all]
