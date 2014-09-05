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

#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

int main()
{
   //! [main]
   //! [all]
   typedef GenSeq::CoprimeIntegers<Compress::NONE> WholeSeq;
   //! [all]

   //! [symmetric]
   typedef GenSeq::CoprimeIntegers<Compress::SYMMETRIC> HalfSeq;
   //! [symmetric]

   for (Modulus n : {7, 8, 12}) {
      std::cout << "lattice size: " << n << std::endl;
      std::cout << "    whole sequence: " << WholeSeq(n) << std::endl;
      std::cout << "     half sequence: " << HalfSeq(n) << std::endl;
   }
   //! [main]

   return 0;
}
