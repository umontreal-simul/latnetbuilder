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

//! [findBest]
// search for the best lattice in the sequence
template <typename SEQ>
typename SEQ::const_iterator findBest(const SEQ& seq)
{
   std::cout << "searching for the best lattice in dimension: " << seq.baseLat().dimension() << std::endl;
   std::cout << "    possible choices: " << seq << std::endl;
   // select the first element
   return seq.begin();
}
//! [findBest]

int main()
{
   SizeParam<LatType::ORDINARY> size(8);
   Dimension dim = 4;

   auto baseLat = createLatDef(size, GeneratingVector{1});

   typedef GenSeq::CoprimeIntegers<Compress::NONE> Coprime;

   //! [loop]
   while (baseLat.dimension() < dim) {
      auto latSeq = LatSeq::cbc(baseLat, Coprime(size));
      auto itBest = findBest(latSeq);
      baseLat = *itBest;
      std::cout << "    selected lattice: " << baseLat << std::endl;
   }
   //! [loop]

   return 0;
}
