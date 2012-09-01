// Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
// 
// This file is part of Lattice Builder.
// 
// Lattice Builder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Lattice Builder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

#include "latbuilder/GenSeq/Extend.h"
#include "latbuilder/Util.h"
#include "latbuilder/TextStream.h"
#include "latbuilder/Digits.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

template <typename SEQ>
void showSeq(Modulus base, const SEQ& seq)
{
   for (auto x : seq) {
      Digits<Modulus> digits(base, x);
      std::cout << "    " << digits << std::endl;
   }
}

int main()
{
   Modulus m = 2;

   //! [main]
   for (Modulus b : {2, 3, 5}) {
      Modulus numPoints = intPow(b, m); // n = b^m
      Modulus gen = numPoints - 2;      // a_j = n - 2 for this example
      std::cout << "base: " << b << std::endl;
      //! [Extend]
      GenSeq::Extend<> seq(b * numPoints, numPoints, gen);
      //! [Extend]
      std::cout << "  one level: " << seq << std::endl;
      showSeq(b, seq);
   }
   //! [main]

   return 0;
}
