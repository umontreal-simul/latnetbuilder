// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
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

#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR113.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

int main()
{
   //! [main]
   //! [types]
   typedef Traversal::Random<LFSR113> Trav;
   typedef GenSeq::GeneratingValues<LatticeType::ORDINARY, Compress::SYMMETRIC, Trav> RandomSeq;
   //! [types]

   size_t r = 4; // 4 random samples

   //! [trav]
   Trav trav(r);
   //! [trav]
   for (uInteger n : {31, 256}) {
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
