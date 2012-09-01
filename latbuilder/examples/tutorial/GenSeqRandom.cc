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
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR258.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

int main()
{
   //! [main]
   //! [types]
   typedef Traversal::Random<LFSR258> Trav;
   typedef GenSeq::CoprimeIntegers<Compress::SYMMETRIC, Trav> RandomSeq;
   //! [types]

   size_t r = 4; // 4 random samples

   //! [trav]
   Trav trav(r);
   //! [trav]
   for (Modulus n : {31, 256}) {
      std::cout << "lattice size: " << n
         << " (" << trav.size() << " random samples)" << std::endl;
      //! [seq]
      RandomSeq seq(n, trav);
      //! [seq]
      std::cout << "          sequence: " << seq << std::endl;
      std::cout << "     same sequence: " << seq << std::endl;
      //! [jump]
      seq.randomGenerator().jump();
      //! [jump]
      std::cout << "    other sequence: " << seq << std::endl;
   }
   //! [main]

   return 0;
}
