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

#include "latbuilder/SizeParam.h"
#include <iostream>

using namespace LatBuilder;

template <LatType L>
void test(const SizeParam<L>& sizeParam)
{
   std::cout << "size parameter: " << sizeParam << std::endl;
   std::cout << "  points:       " << sizeParam.numPoints() << std::endl;
   std::cout << "  totient:      " << sizeParam.totient() << std::endl;
}

int main(int argc, const char *argv[])
{
   test(SizeParam<LatType::ORDINARY>(100));
   test(SizeParam<LatType::EMBEDDED>(256));
   return 0;
}
