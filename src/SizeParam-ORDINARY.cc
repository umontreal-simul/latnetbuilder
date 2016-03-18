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

