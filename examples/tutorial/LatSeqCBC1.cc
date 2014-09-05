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
#include "latbuilder/LatSeq/CBC.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

int main()
{
   //! [main]
   SizeParam<LatType::ORDINARY> size(8);

   //! [baseLat]
   auto baseLat = createLatDef(size, GeneratingVector{1, 5});
   //! [baseLat]

   typedef GenSeq::CoprimeIntegers<Compress::NONE> Coprime;

   //! [latSeq]
   auto latSeq = LatSeq::cbc(baseLat, Coprime(size));
   //! [latSeq]

   //! [output]
   std::cout << latSeq << std::endl;
   //! [output]
   //! [main]

   return 0;
}
