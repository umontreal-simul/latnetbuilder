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
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/GenSeq/Creator.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR113.h"
#include "latbuilder/TextStream.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

//! [nonrandom]
template <Lattice LA>
void SeqVector(typename LatticeTraits<LA>::Modulus modulus){
   typedef GenSeq::GeneratingValues<LA, Compress::NONE> Seq;
   SizeParam<LA, LatType::ORDINARY> n(modulus);      // lattice size
   SizeParam<LA, LatType::ORDINARY> n0(LatticeTraits<LA>::TrivialModulus);   // fake lattice size to obtain a single value (1)
   Dimension dim = 3;
   //! [VectorCreator]
   auto vec = GenSeq::VectorCreator<Seq>::create(n, dim);
   //! [VectorCreator]
   //! [1st]
   vec[0] = GenSeq::Creator<Seq>::create(n0);
   //! [1st]
   std::cout << "lattice size: " << n << std::endl;
   std::cout << "    generating value sequences: " << vec << std::endl;

}
//! [nonrandom]

//! [random]
template <Lattice LA>
void RandomSeqVector(typename LatticeTraits<LA>::Modulus modulus){
   typedef GenSeq::GeneratingValues<LA, Compress::NONE, Traversal::Random<LFSR113>> RandomSeq;
   SizeParam<LA, LatType::ORDINARY> n(modulus);      // lattice size
   SizeParam<LA, LatType::ORDINARY> n0(LatticeTraits<LA>::TrivialModulus);   // fake lattice size to obtain a single value (1)
   Dimension dim = 3;
  
   auto randVec = GenSeq::VectorCreator<RandomSeq>::create(n, dim, 5);
   randVec[0] = GenSeq::Creator<RandomSeq>::create(n0, 1); // replace 1st with singleton
   std::cout << "lattice size: " << n << std::endl;
   std::cout << "    random generating value sequences: " << randVec << std::endl;

}
//! [random]

int main()
{
   //! [nonrandommain]
   SeqVector<Lattice::INTEGRATION>(7);
   SeqVector<Lattice::POLYNOMIAL>(PolynomialFromInt(7));
   //! [nonrandommain]
   //! [randommain]
   RandomSeqVector<Lattice::INTEGRATION>(7);
   RandomSeqVector<Lattice::POLYNOMIAL>(PolynomialFromInt(7));
   //! [randommain]

   
   return 0;
}
