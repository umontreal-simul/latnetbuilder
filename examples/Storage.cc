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
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/GenSeq/Creator.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR258.h"
#include "latbuilder/TextStream.h"
#include "latbuilder/Storage.h"
#include "latbuilder/LatSeq/Combiner.h"   
#include "latbuilder/LatSeq/Korobov.h"
#include "latbuilder/LatSeq/CBC.h"
#include <iostream>
using namespace LatBuilder;
using TextStream::operator<<;




template <LatticeType LA, PointSetType L, Compress C, PerLevelOrder PLO>
void test(const Storage<LA, L, C, PLO>& storage, typename LatticeTraits<LA>::GenValue stride_param)
{
   std::cout << "==> storage / compression: " << Storage<LA, L, C, PLO>::name() << std::endl;
   std::cout << "    virtual / actual sizes: " << storage.virtualSize() << " / " << storage.size() << std::endl;
   RealVector original(storage.size());
   auto unpermuted = storage.unpermuted(original);
   for (RealVector::size_type j = unpermuted.size(); j > 0; j--)
      unpermuted[j - 1] = j - 1;
   auto strided = storage.strided(original, stride_param);
   std::cout << "    original:   " << original << std::endl;
   std::cout << "    unpermuted: " << unpermuted << std::endl;
   std::cout << "    strided(3): " << strided << std::endl;
}
int main(int argc, const char *argv[])
{

   test(Storage<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::NONE     >(12), 3);
   test(Storage<LatticeType::ORDINARY, PointSetType::UNILEVEL, Compress::SYMMETRIC>(12), 3);
   test(Storage<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::NONE     >(16), 3);
   test(Storage<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::SYMMETRIC>(16), 3); 
   test(Storage<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::NONE     ,PerLevelOrder::BASIC>(16), 3);
   test(Storage<LatticeType::ORDINARY, PointSetType::MULTILEVEL, Compress::SYMMETRIC,PerLevelOrder::BASIC>(16), 3); 

   test(Storage<LatticeType::POLYNOMIAL, PointSetType::UNILEVEL, Compress::NONE>(PolynomialFromInt(13)), PolynomialFromInt(3));
   test(Storage<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL, Compress::NONE>(PolynomialFromInt(13)), PolynomialFromInt(3));
   test(Storage<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC>(PolynomialFromInt(13)), PolynomialFromInt(3)); //irreductible modulus
   return 0;
}
