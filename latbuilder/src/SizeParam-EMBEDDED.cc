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

#include "latbuilder/SizeParam.h"
#include "latbuilder/Util.h"
#include "latcommon/UtilLC.h"

namespace LatBuilder {

SizeParam<LatType::EMBEDDED>::SizeParam(Modulus primeBase, Level maxLevel):
   BasicSizeParam<SizeParam<LatType::EMBEDDED>>(primeBase == 0 ? 0 : intPow(primeBase, maxLevel)),
   m_base(primeBase),
   m_maxLevel(maxLevel)
{
   if (primeBase >= 2 and !LatCommon::IsPrime(primeBase))
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

