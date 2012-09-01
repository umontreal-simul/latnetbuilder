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
#include "latbuilder/GenSeq/CyclicGroup.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/GenSeq/PowerSeq.h"
#include "latbuilder/GenSeq/Creator.h"
#include "latbuilder/GenSeq/VectorCreator.h"

#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR258.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;

template <class GENSEQ, class SIZE, typename... ARGS>
void test(const std::initializer_list<SIZE>& init, ARGS&&... args) {
   std::cout << "==> " << GENSEQ::name() << std::endl;
   for (SIZE size : init) {
      auto seq = GenSeq::Creator<GENSEQ>::create(size, std::forward<ARGS>(args)...);
      auto vec = GenSeq::VectorCreator<GENSEQ>::create(size, 2, std::forward<ARGS>(args)...);
      auto seq2 = GenSeq::Creator<GenSeq::PowerSeq<GENSEQ>>::create(size, 2, std::forward<ARGS>(args)...);
      using TextStream::operator<<;
      std::cout << size << ": " << seq << std::endl;
      std::cout << size << ": " << vec << " (vector)" << std::endl;
      std::cout << size << ": " << seq2 << " (squared)" << std::endl;
   }
}


int main()
{
   using namespace LatBuilder::GenSeq;

   auto init1 = std::initializer_list<SizeParam<LatType::ORDINARY>>{5, 6, 7, 8, 9, 10, 11, 12};
   auto init2 = std::initializer_list<SizeParam<LatType::EMBEDDED>>{{2,5}, {3,3}, {11,1}};
   auto init3 = std::initializer_list<SizeParam<LatType::ORDINARY>>{256, 257, 1000};
   auto init4 = std::initializer_list<SizeParam<LatType::EMBEDDED>>{{2,8}, {3,5}};

   test<CoprimeIntegers<Compress::NONE>>(init1);
   test<CoprimeIntegers<Compress::SYMMETRIC>>(init1);

   test<CoprimeIntegers<Compress::NONE>>(init2);
   test<CoprimeIntegers<Compress::SYMMETRIC>>(init2);

   test<CyclicGroup<Compress::NONE>>(init2);
   test<CyclicGroup<Compress::SYMMETRIC>>(init2);

   test<CoprimeIntegers<Compress::NONE, Traversal::Random<LFSR258>>>(init3, 5);
   test<CoprimeIntegers<Compress::SYMMETRIC, Traversal::Random<LFSR258>>>(init3, 5);

   test<CyclicGroup<Compress::NONE, Traversal::Random<LFSR258>>>(init4, 5);
   test<CyclicGroup<Compress::SYMMETRIC, Traversal::Random<LFSR258>>>(init4, 5);

   return 0;
}
