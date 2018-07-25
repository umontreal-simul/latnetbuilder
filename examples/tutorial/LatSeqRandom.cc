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
#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR258.h"
#include "latbuilder/TextStream.h"

#include "Path.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;



//! [genSeqs]
template <LatticeType LA>
void test(typename LatticeTraits<LA>::Modulus modulus){
   SizeParam<LA, EmbeddingType::UNILEVEL> size(modulus);      
   SizeParam<LA, EmbeddingType::UNILEVEL> size0(LatticeTraits<LA>::TrivialModulus);   
   Dimension dim = 3;
   size_t r = 4; // 4 random samples

   typedef GenSeq::GeneratingValues<LA, Compress::NONE, Traversal::Random<LFSR258>> Coprime;
   auto genSeqs = GenSeq::VectorCreator<Coprime>::create(size, dim,r);
   // consider only 1 for 1st coordinate
   genSeqs[0] = GenSeq::Creator<Coprime>::create(size0);
   //! [genSeqs]
   //! [latSeq]
   auto latSeq = LatSeq::combine<Zip>(size, genSeqs);

   std::cout << latSeq << std::endl;
   
}
//! [latSeq]

int main()
{
   
   SET_PATH_TO_LATNETBUILDER_FOR_EXAMPLES();
   //! [output]
   test<LatticeType::ORDINARY>(31);
   //! [output]
   //! [poutput]
   test<LatticeType::POLYNOMIAL>(PolynomialFromInt(31));
   //! [poutput]

   return 0;
}