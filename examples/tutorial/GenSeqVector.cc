// Copyright (c) 2012, 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/GenSeq/Creator.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR113.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

int main()
{
   //! [main]
   typedef GenSeq::CoprimeIntegers<Compress::NONE> Seq;

   SizeParam<LatType::ORDINARY> n(7);      // lattice size
   SizeParam<LatType::ORDINARY> n0(2);     // fake lattice size to obtain a single value (1)
   Dimension dim = 3;           // dimension

   //! [VectorCreator]
   auto vec = GenSeq::VectorCreator<Seq>::create(n, dim);
   //! [VectorCreator]
   //! [1st]
   vec[0] = GenSeq::Creator<Seq>::create(n0);
   //! [1st]
   std::cout << "lattice size: " << n << std::endl;
   std::cout << "    integer sequences: " << vec << std::endl;
   //! [main]

   //! [random]
   n = 31;
   typedef GenSeq::CoprimeIntegers<Compress::NONE, Traversal::Random<LFSR113>> RandomSeq;
   auto randVec = GenSeq::VectorCreator<RandomSeq>::create(n, dim, 5);
   randVec[0] = GenSeq::Creator<RandomSeq>::create(n0, 1); // replace 1st with singleton
   std::cout << "lattice size: " << n << std::endl;
   std::cout << "    random integer sequences: " << randVec << std::endl;
   //! [random]

   return 0;
}
