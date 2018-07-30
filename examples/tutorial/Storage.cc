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

#include "latbuilder/Storage.h"
#include "latbuilder/SizeParam.h"

#include "Path.h"

#include <iostream>

using namespace LatBuilder;

//! [all]
template <LatticeType LA, EmbeddingType ET, Compress COMP>
void test(typename LatticeTraits<LA>::Modulus modulus)
{
   SizeParam<LA, ET> size(modulus);
   Storage<LA, ET, COMP> storage(size);
   std::cout << "storage name: " << storage.name() << std::endl;
   std::cout << "  size parameter: " << storage.sizeParam() << std::endl;
   std::cout << "  virtual size:   " << storage.virtualSize() << std::endl;
   std::cout << "  actual size:    " << storage.size() << std::endl;
}

int main()
{
   SET_PATH_TO_LATNETBUILDER_FOR_EXAMPLES();
   uInteger n = 16;
   Polynomial P = PolynomialFromInt(7);
   test<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::NONE>(n);
   test<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::NONE>(n);
   test<LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC>(n);
   test<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::SYMMETRIC>(n);

   test<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE>(P);
   test<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE>(P);

   return 0;
}
//! [all]
