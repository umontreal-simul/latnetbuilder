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

#include "latbuilder/LatDef.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/TextStream.h"
#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

//! [printLatDef]
template <LatType L>
void printLatDef(const LatDef<L>& def)
{
   std::cout << "    dimension:         " << def.dimension() << std::endl;
   std::cout << "    size parameter:    " << def.sizeParam() << std::endl;
   std::cout << "    generating vector: " << def.gen() << std::endl;
   std::cout << "    definition:        " << def << std::endl;
}
//! [printLatDef]

int main()
{
   //! [ordinary]
   auto ordinary = createLatDef(SizeParam<LatType::ORDINARY>(31), {1, 12, 3});
   std::cout << "ordinary lattice:" << std::endl;
   printLatDef(ordinary);
   //! [ordinary]

   //! [embedded]
   auto embedded = createLatDef(SizeParam<LatType::EMBEDDED>(2, 5), {1, 7, 9});
   std::cout << "embedded lattice:" << std::endl;
   printLatDef(embedded);
   //! [embedded]

   return 0;
}
