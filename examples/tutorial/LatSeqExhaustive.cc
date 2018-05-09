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
#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

//! [genSeqs]
template <LatticeType LA>
void test(typename LatticeTraits<LA>::Modulus modulus){
   SizeParam<LA, PointSetType::UNILEVEL> size(modulus);      
   SizeParam<LA, PointSetType::UNILEVEL> size0(LatticeTraits<LA>::TrivialModulus);   
   Dimension dim = 3;

   typedef GenSeq::GeneratingValues<LA, Compress::NONE> Coprime;
   auto genSeqs = GenSeq::VectorCreator<Coprime>::create(size, dim);
   // consider only 1 for 1st coordinate
   genSeqs[0] = GenSeq::Creator<Coprime>::create(size0);
   //! [genSeqs]
   //! [latSeq]
   auto latSeq = LatSeq::combine<CartesianProduct>(size, genSeqs);

   std::cout << latSeq << std::endl;
   
}
//! [latSeq]

int main()
{
   

   //! [output]
   test<LatticeType::ORDINARY>(8);
   //! [output]
   //! [poutput]
   test<LatticeType::POLYNOMIAL>(PolynomialFromInt(7));
   //! [poutput]

   return 0;
}
