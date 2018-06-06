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
#include "latbuilder/LatDef.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/TextStream.h"
#include <iostream>
using namespace LatBuilder;
using TextStream::operator<<;

//! [printLatDef]
template <LatticeType LA, PointSetType L>
void printLatDef(const LatDef<LA, L>& def)
{
   std::cout << "    dimension:         " << def.dimension() << std::endl;
   std::cout << "    size parameter:    " << def.sizeParam() << std::endl;
   std::cout << "    generating vector: " << def.gen() << std::endl;
   std::cout << "    definition:        " << def << std::endl;
}
//! [printLatDef]
int main()
{
   //! [ordinary]
   auto ordinary = createLatDef(SizeParam<LatticeType::ORDINARY, PointSetType::UNILEVEL>(31), {1, 12, 3});
   std::cout << "ordinary - simple lattice:" << std::endl;
   printLatDef(ordinary);
    //! [ordinary]

    //! [embedded]
   auto embedded = createLatDef(SizeParam<LatticeType::ORDINARY, PointSetType::MULTILEVEL>(2, 5), {1, 7, 9});
   std::cout << "ordinary - embedded lattice:" << std::endl;
   printLatDef(embedded);
   //! [embedded]

   //! [pordinary]
   Polynomial P = PolynomialFromInt(13); // P = 1 + z^2 + z^3
   auto pordinary = createLatDef(SizeParam<LatticeType::POLYNOMIAL, PointSetType::UNILEVEL>(P), {PolynomialFromInt(1), PolynomialFromInt(5), PolynomialFromInt(3)});
   std::cout << "polynomial - simple lattice:" << std::endl;
   printLatDef(pordinary);
   //! [pordinary]

   //! [pembedded]
   auto pembedded = createLatDef(SizeParam<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL>(PolynomialFromInt(2), 5),{PolynomialFromInt(1), PolynomialFromInt(10), PolynomialFromInt(3)});
   std::cout << "polynomial - embedded lattice:" << std::endl;
   printLatDef(pembedded);
   //! [pembedded]

   return 0;
}