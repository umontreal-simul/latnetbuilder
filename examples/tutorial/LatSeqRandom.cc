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

#include "latbuilder/SizeParam.h"
#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR113.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

int main()
{
   //! [genSeqs]
   SizeParam<LatType::ORDINARY> size(31);
   SizeParam<LatType::ORDINARY> size0(2);
   Dimension dim = 3;
   size_t r = 4; // 4 random samples

   typedef GenSeq::CoprimeIntegers<Compress::NONE, Traversal::Random<LFSR113>> Coprime;
   auto genSeqs = GenSeq::VectorCreator<Coprime>::create(size, dim, r);
   // consider only 1 for 1st coordinate
   genSeqs[0] = GenSeq::Creator<Coprime>::create(size0);
   //! [genSeqs]

   //! [latSeq]
   auto latSeq = LatSeq::combine<Zip>(size, genSeqs);
   //! [latSeq]

   //! [output]
   std::cout << latSeq << std::endl;
   //! [output]

   return 0;
}
