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

#include "latbuilder/GenSeq/Extend.h"
#include "latbuilder/Util.h"
#include "latbuilder/TextStream.h"
#include "latbuilder/Digits.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

template <typename SEQ>
void showSeq(uInteger base, const SEQ& seq)
{
   for (auto x : seq) {
      Digits<uInteger> digits(base, x);
      std::cout << "    " << digits << std::endl;
   }
}

int main()
{
   uInteger m = 2;

   //! [main]
   for (uInteger b : {2, 3, 5}) {
      uInteger numPoints = intPow(b, m); // n = b^m
      uInteger gen = numPoints - 2;      // a_j = n - 2 for this example
      std::cout << "base: " << b << std::endl;
      //! [Extend]
      GenSeq::Extend<LatticeType::ORDINARY> seq(b * numPoints, numPoints, gen);
      //! [Extend]
      std::cout << "  one level: " << seq << std::endl;
      showSeq(b, seq);
   }

   Polynomial base = PolynomialFromInt(7);
   Polynomial P = intPow(base,3);
   Polynomial generator = PolynomialFromInt(5);
   //! [pExtend]
   GenSeq::Extend<LatticeType::POLYNOMIAL> seq(base * P, P, generator);
   //! [pExtend]
   //! [main]

   return 0;
}
