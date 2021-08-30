// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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
#include "latbuilder/TextStream.h"

#include "Path.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

int main()
{
   SET_PATH_TO_LATNETBUILDER_FOR_EXAMPLES();
   //! [all]
   typedef GenSeq::GeneratingValues<LatticeType::ORDINARY, Compress::NONE> WholeSeqIntegers;
   //! [all]

   //! [symmetric]
   typedef GenSeq::GeneratingValues<LatticeType::ORDINARY, Compress::SYMMETRIC> HalfSeqIntegers;
   //! [symmetric]

   //! [pall]
   typedef GenSeq::GeneratingValues<LatticeType::POLYNOMIAL, Compress::NONE> WholeSeqPolynomials;
   //! [pall]

   //! [main]
   for (uInteger n : {7, 8, 12}) {
      std::cout << "lattice size: " << n << std::endl;
      std::cout << "    whole sequence: " << WholeSeqIntegers(n) << std::endl;
      std::cout << "     half sequence: " << HalfSeqIntegers(n) << std::endl;
   }
   //! [main]

   //! [pmain]
   for (Polynomial P : {PolynomialFromInt(10), PolynomialFromInt(13)}) {
      std::cout << "lattice size: " << P << std::endl;
      std::cout << "    whole sequence: " << WholeSeqPolynomials(P) << std::endl;
      
   }
   //! [pmain]

   return 0;
}
