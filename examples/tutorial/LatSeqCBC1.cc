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
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;


//! [main]
template<LatticeType LA>
void test(typename LatticeTraits<LA>::Modulus modulus, typename LatticeTraits<LA>::GeneratingVector genv){
   SizeParam<LA, PointSetType::UNILEVEL> size(modulus);
   
   //! [baseLat]
   auto baseLat = createLatDef(size, genv);
   //! [baseLat]

   typedef GenSeq::GeneratingValues<LA, Compress::NONE> Coprime;

   //! [latSeq]
   auto latSeq = LatSeq::cbc(baseLat, Coprime(size));
   //! [latSeq]

   std::cout << latSeq << std::endl;
}
//! [main]

int main()
{
   
   //! [output]
   test<LatticeType::ORDINARY>(8, {1,5});
   test<LatticeType::POLYNOMIAL>(PolynomialFromInt(7), {PolynomialFromInt(1),PolynomialFromInt(5)});
   //! [output]


   return 0;
}