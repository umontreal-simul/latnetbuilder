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
#include "latbuilder/Util.h"
#include "latcommon/Util.h"
#include "latcommon/IntFactor.h"

namespace LatBuilder {

SizeParam<LatType::EMBEDDED>::SizeParam(Modulus primeBase, Level maxLevel):
   BasicSizeParam<SizeParam<LatType::EMBEDDED>>(primeBase == 0 ? 0 : intPow(primeBase, maxLevel)),
   m_base(primeBase),
   m_maxLevel(maxLevel)
{
   if (primeBase >= 2 and LatCommon::IntFactor::isPrime(primeBase, 0) == LatCommon::COMPOSITE)
      throw std::invalid_argument("SizeParam: primeBase is not prime");
}

SizeParam<LatType::EMBEDDED>::SizeParam(Modulus numPoints):
   BasicSizeParam<SizeParam<LatType::EMBEDDED>>(numPoints)
{
   if (numPoints == 0) {
      m_base = 0;
      m_maxLevel = 0;
   }
   else if (numPoints == 1) {
      m_base = 1;
      m_maxLevel = 0;
   }
   else {
      const auto factors = primeFactorsMap(numPoints);
      if (factors.size() != 1)
         throw std::runtime_error("not an integer power of a prime base");
      const auto& factor = *factors.begin();
      m_base = factor.first;
      m_maxLevel = factor.second;
   }
}

Modulus
SizeParam<LatType::EMBEDDED>::numPointsOnLevel(Level level) const
{
   if (level > maxLevel())
      throw std::invalid_argument("level > maxLevel");
   return base() == 0 ? 0 : intPow(base(), level);
}

size_t
SizeParam<LatType::EMBEDDED>::totient() const
{ return base() == 0 ? 0 : (base() - 1) * this->numPoints() / base(); }

void
SizeParam<LatType::EMBEDDED>::normalize(Real& merit) const
{ merit /= this->numPoints(); }

void
SizeParam<LatType::EMBEDDED>::normalize(RealVector& merit) const
{
   if (merit.size() != maxLevel() + 1)
      throw std::logic_error("merit vector size and maximum level do not match");
   for (Level level = 0; level < merit.size(); level++)
      merit[level] /= numPointsOnLevel(level);
}

std::ostream&
SizeParam<LatType::EMBEDDED>::format(std::ostream& os) const
{ os << base(); if (maxLevel() != 1) os << "^" << maxLevel(); return os; }

}

