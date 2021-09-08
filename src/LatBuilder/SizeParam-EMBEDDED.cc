// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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
#include "latticetester/Util.h"
#include "latticetester/IntFactor.h"
#include <NTL/GF2XFactoring.h>

namespace LatBuilder {

//===============================================================================================================
template<>
SizeParam<LatticeType::ORDINARY,EmbeddingType::MULTILEVEL>::SizeParam(uInteger primeBase, Level maxLevel):
   BasicSizeParam<SizeParam<LatticeType::ORDINARY,EmbeddingType::MULTILEVEL>>(primeBase == 0 ? 0 : intPow(primeBase, maxLevel)),
   m_base(primeBase),
   m_maxLevel(maxLevel)
{
   if (primeBase >= 2 and LatticeTester::IntFactor<std::int64_t>::isPrime(primeBase, 0) == LatticeTester::COMPOSITE)
      throw std::invalid_argument("SizeParam: primeBase is not prime");
}

template<>
SizeParam<LatticeType::POLYNOMIAL,EmbeddingType::MULTILEVEL>::SizeParam(Polynomial primeBase, Level maxLevel):
   BasicSizeParam<SizeParam<LatticeType::POLYNOMIAL,EmbeddingType::MULTILEVEL>>(IsZero(primeBase) ? Polynomial(0) : intPow(primeBase, maxLevel)),
   m_base(primeBase),
   m_maxLevel(maxLevel)
{
   if ( !IterIrredTest(primeBase))
      throw std::invalid_argument("SizeParam: primeBase is not prime");

}

template<>
SizeParam<LatticeType::DIGITAL,EmbeddingType::MULTILEVEL>::SizeParam(uInteger primeBase, Level maxLevel):
   BasicSizeParam<SizeParam<LatticeType::DIGITAL,EmbeddingType::MULTILEVEL>>(primeBase == 0 ? 0 : intPow(primeBase, maxLevel)),
   m_base(primeBase),
   m_maxLevel(maxLevel)
{
   if (primeBase >= 2 and LatticeTester::IntFactor<std::int64_t>::isPrime(primeBase, 0) == LatticeTester::COMPOSITE)
      throw std::invalid_argument("SizeParam: primeBase is not prime");
}

//===================================================================================================================

template<>
SizeParam<LatticeType::ORDINARY,EmbeddingType::MULTILEVEL>::SizeParam(uInteger numPoints):
   BasicSizeParam<SizeParam<LatticeType::ORDINARY,EmbeddingType::MULTILEVEL>>(numPoints)
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

template<>
SizeParam<LatticeType::POLYNOMIAL,EmbeddingType::MULTILEVEL>::SizeParam(Polynomial modulus):
   BasicSizeParam<SizeParam<LatticeType::POLYNOMIAL,EmbeddingType::MULTILEVEL>>(modulus)
{
   if (IsZero(modulus)) {
      m_base = Polynomial(0);
      m_maxLevel = 0;
   }
   else {
      NTL::vector< NTL::Pair< NTL::GF2X, long > > factors ;
      CanZass(factors, modulus); // calls "Cantor/Zassenhaus" algorithm from <NTL/GF2XFactoring.h>
      if (factors.size() != 1)
         throw std::runtime_error("not an integer power of a prime base");
      const auto& factor = *factors.begin();
      m_base = factor.a; // = factor.first
      m_maxLevel = factor.b; // = factor.second
   }
}

template<>
SizeParam<LatticeType::DIGITAL,EmbeddingType::MULTILEVEL>::SizeParam(uInteger numPoints):
   BasicSizeParam<SizeParam<LatticeType::DIGITAL,EmbeddingType::MULTILEVEL>>(numPoints)
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

//=======================================================================================================================

template<>
SizeParam<LatticeType::ORDINARY,EmbeddingType::MULTILEVEL>::size_type
SizeParam<LatticeType::ORDINARY,EmbeddingType::MULTILEVEL>::numPointsOnLevel(Level level) const
{
   if (level > maxLevel())
      throw std::invalid_argument("level > maxLevel");
   return base() == 0 ? 0 : intPow(base(), level);
}

template<>
SizeParam<LatticeType::POLYNOMIAL,EmbeddingType::MULTILEVEL>::size_type
SizeParam<LatticeType::POLYNOMIAL,EmbeddingType::MULTILEVEL>::numPointsOnLevel(Level level) const
{
   if (level > maxLevel())
      throw std::invalid_argument("level > maxLevel");
   return IsZero(base())  ? 0 : intPow( 2, deg(base())*level );
}

template<>
SizeParam<LatticeType::DIGITAL,EmbeddingType::MULTILEVEL>::size_type
SizeParam<LatticeType::DIGITAL,EmbeddingType::MULTILEVEL>::numPointsOnLevel(Level level) const
{
   if (level > maxLevel())
      throw std::invalid_argument("level > maxLevel");
   return base() == 0 ? 0 : intPow(base(), level);
}
//========================================================================================================================

template<>
size_t
SizeParam<LatticeType::ORDINARY,EmbeddingType::MULTILEVEL>::totient() const
{ return base() == 0 ? 0 : (base() - 1) * this->numPoints() / base(); }

template<>
size_t
SizeParam<LatticeType::POLYNOMIAL,EmbeddingType::MULTILEVEL>::totient() const
{ return IsZero(base()) == 0 ? 0 : (intPow(2,deg(base())) - 1) * this->numPoints() / intPow(2,deg(base())); }

//=========================================================================================================================

template<LatticeType LR>
void
SizeParam<LR,EmbeddingType::MULTILEVEL>::normalize(Real& merit) const
{ merit /= this->numPoints(); }

template<LatticeType LR>
void
SizeParam<LR,EmbeddingType::MULTILEVEL>::normalize(RealVector& merit) const
{
   if (merit.size() != maxLevel() + 1)
      throw std::logic_error("merit vector size and maximum level do not match");
   for (Level level = 0; level < merit.size(); level++)
      merit[level] /= numPointsOnLevel(level);
}
//===========================================================================================================================
template<LatticeType LR>
std::ostream&
SizeParam<LR,EmbeddingType::MULTILEVEL>::format(std::ostream& os) const
{ os << base(); if (maxLevel() != 1) os << "^" << maxLevel(); return os; }
//===========================================================================================================================

template class SizeParam<LatticeType::ORDINARY,EmbeddingType::MULTILEVEL>;
template class SizeParam<LatticeType::POLYNOMIAL,EmbeddingType::MULTILEVEL>;
template class SizeParam<LatticeType::DIGITAL,EmbeddingType::MULTILEVEL>;

}

