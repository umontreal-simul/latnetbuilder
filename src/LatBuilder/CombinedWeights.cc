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

#include "latbuilder/CombinedWeights.h"
#include "latbuilder/TextStream.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

namespace LatBuilder
{

//===========================================================================

CombinedWeights::CombinedWeights ():
   Weights()
{}

//===========================================================================

LatticeTester::Weight CombinedWeights::getWeight (const LatticeTester::Coordinates& projection) const
{
   LatticeTester::Weight sum = 0.0;
   for (const auto& w : m_weights)
      sum += w->getWeight(projection);
   return sum;
}

//===========================================================================

void CombinedWeights::format(ostream& os) const
{
   using TextStream::operator<<;
   if (m_weights.size() > 1){
      os << "CombinedWeights(";
   }
   auto it = m_weights.begin();
   if (it != m_weights.end()) {
      os << **it;
      while (++it != m_weights.end())
         os << ", " << **it;
   }
   if (m_weights.size() > 1){
      os << ")";
   }
}

//===========================================================================
// non-member functions
//===========================================================================

} // namespace
