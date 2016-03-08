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

namespace LatBuilder {

SizeParam<LatType::ORDINARY>::SizeParam(Modulus numPoints):
   BasicSizeParam<SizeParam<LatType::ORDINARY>>(numPoints)
{}

size_t
SizeParam<LatType::ORDINARY>::totient() const
{
   auto n = numPoints();
   for (const auto& p : LatBuilder::primeFactors(n))
      n = n * (p - 1) / p;
   return n;
}

void
SizeParam<LatType::ORDINARY>::normalize(Real& merit) const
{ merit /= numPoints(); }

void
SizeParam<LatType::ORDINARY>::normalize(RealVector& merit) const
{ merit /= numPoints(); }

std::ostream&
SizeParam<LatType::ORDINARY>::format(std::ostream& os) const
{ return os << numPoints(); }

}

