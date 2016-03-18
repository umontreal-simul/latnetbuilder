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
