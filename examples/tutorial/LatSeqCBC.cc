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

//! [findBest]
// search for the best lattice in the sequence
template <typename SEQ>
typename SEQ::const_iterator findBest(const SEQ& seq)
{
   std::cout << "searching for the best lattice in dimension: " << seq.baseLat().dimension() << std::endl;
   std::cout << "    possible choices: " << seq << std::endl;
   // select the first element
   return seq.begin();
}
//! [findBest]

template<LatticeType LA>
void CBCsearch(typename LatticeTraits<LA>::Modulus modulus){
   SizeParam<LA, LatEmbed::SIMPLE> size(modulus);
   Dimension dim = 4;

   auto baseLat = createLatDef(size, typename LatticeTraits<LA>::GeneratingVector{typename LatticeTraits<LA>::GenValue(1)});

   typedef GenSeq::GeneratingValues<LA, Compress::NONE> Coprime;

   //! [loop]
   while (baseLat.dimension() < dim) {
      auto latSeq = LatSeq::cbc(baseLat, Coprime(size));
      auto itBest = findBest(latSeq);
      baseLat = *itBest;
      std::cout << "    selected lattice: " << baseLat << std::endl;
   }
   //! [loop]

}
int main()
{
   CBCsearch<LatticeType::ORDINARY>(8);
   CBCsearch<LatticeType::POLYNOMIAL>(PolynomialFromInt(7));

   return 0;
}
