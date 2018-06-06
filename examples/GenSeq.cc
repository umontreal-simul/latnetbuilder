// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "latbuilder/SizeParam.h"
#include "latbuilder/GenSeq/CyclicGroup.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/GenSeq/PowerSeq.h"
#include "latbuilder/GenSeq/Creator.h"
#include "latbuilder/GenSeq/VectorCreator.h"

#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR113.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;

template <class GENSEQ, class SIZE, typename... ARGS>
void test(const std::vector<SIZE>& init, ARGS&&... args) {
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

   auto init1 = std::vector<SizeParam<LatType::ORDINARY>>{5, 6, 7, 8, 9, 10, 11, 12};
   auto init2 = std::vector<SizeParam<LatType::EMBEDDED>>{{2,5}, {3,3}, {11,1}};
   auto init3 = std::vector<SizeParam<LatType::ORDINARY>>{256, 257, 1000};
   auto init4 = std::vector<SizeParam<LatType::EMBEDDED>>{{2,8}, {3,5}};

   test<CoprimeIntegers<Compress::NONE>>(init1);
   test<CoprimeIntegers<Compress::SYMMETRIC>>(init1);

   test<CoprimeIntegers<Compress::NONE>>(init2);
   test<CoprimeIntegers<Compress::SYMMETRIC>>(init2);

   test<CyclicGroup<Compress::NONE>>(init2);
   test<CyclicGroup<Compress::SYMMETRIC>>(init2);

   test<CoprimeIntegers<Compress::NONE, Traversal::Random<LFSR113>>>(init3, 5);
   test<CoprimeIntegers<Compress::SYMMETRIC, Traversal::Random<LFSR113>>>(init3, 5);

   test<CyclicGroup<Compress::NONE, Traversal::Random<LFSR113>>>(init4, 5);
   test<CyclicGroup<Compress::SYMMETRIC, Traversal::Random<LFSR113>>>(init4, 5);

   return 0;
}
