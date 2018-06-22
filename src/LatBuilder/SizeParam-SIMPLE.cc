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

#include "latbuilder/SizeParam.h"
#include "latbuilder/Util.h"
#include <NTL/GF2XFactoring.h>

namespace LatBuilder {

//===========================================================================================
template <LatticeType LR>
SizeParam<LR,EmbeddingType::UNILEVEL>::SizeParam(SizeParam<LR,EmbeddingType::UNILEVEL>::Modulus modulus):
   BasicSizeParam<SizeParam<LR,EmbeddingType::UNILEVEL>>(modulus)
{}

//===========================================================================================

template<>
size_t
SizeParam<LatticeType::ORDINARY,EmbeddingType::UNILEVEL>::totient() const
{
   auto n = numPoints();
   for (const auto& p : LatBuilder::primeFactors(n))
      n = n * (p - 1) / p;
   return n;
}

template<>
size_t
SizeParam<LatticeType::POLYNOMIAL,EmbeddingType::UNILEVEL>::totient() const
{
   auto polynomial = modulus();
   auto n = intPow(2,deg(polynomial));
   NTL::vector< NTL::Pair< Polynomial, long > > factors ;
   CanZass(factors, polynomial); // calls "Cantor/Zassenhaus" algorithm from <NTL/GF2XFactoring.h>
   for (const auto& p : factors)
      n = n * (intPow(2,deg(p.a)) - 1) / intPow(2,deg(p.a));
   return n;
}

//================================================================================================

template <LatticeType LR>
void
SizeParam<LR,EmbeddingType::UNILEVEL>::normalize(Real& merit) const
{ merit /= this->numPoints(); }

template <LatticeType LR>
void
SizeParam<LR,EmbeddingType::UNILEVEL>::normalize(RealVector& merit) const
{ merit /= this->numPoints(); }

template <LatticeType LR>
std::ostream&
SizeParam<LR,EmbeddingType::UNILEVEL>::format(std::ostream& os) const
{ return os << this->modulus(); }

//==================================================================================================

template class SizeParam<LatticeType::ORDINARY,EmbeddingType::UNILEVEL>;
template class SizeParam<LatticeType::POLYNOMIAL,EmbeddingType::UNILEVEL>;
template class SizeParam<LatticeType::DIGITAL,EmbeddingType::UNILEVEL>;

}

