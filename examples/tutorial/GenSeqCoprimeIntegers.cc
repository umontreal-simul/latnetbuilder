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
