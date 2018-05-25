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
#include "latticetester/Util.h"
#include "latticetester/IntFactor.h"
#include <NTL/GF2XFactoring.h>

namespace LatBuilder {

//===============================================================================================================
template<>
SizeParam<LatticeType::ORDINARY,PointSetType::MULTILEVEL>::SizeParam(uInteger primeBase, Level maxLevel):
   BasicSizeParam<SizeParam<LatticeType::ORDINARY,PointSetType::MULTILEVEL>>(primeBase == 0 ? 0 : intPow(primeBase, maxLevel)),
   m_base(primeBase),
   m_maxLevel(maxLevel)
{
   if (primeBase >= 2 and LatticeTester::IntFactor::isPrime(primeBase, 0) == LatticeTester::COMPOSITE)
      throw std::invalid_argument("SizeParam: primeBase is not prime");
}

template<>
SizeParam<LatticeType::POLYNOMIAL,PointSetType::MULTILEVEL>::SizeParam(Polynomial primeBase, Level maxLevel):
   BasicSizeParam<SizeParam<LatticeType::POLYNOMIAL,PointSetType::MULTILEVEL>>(IsZero(primeBase) ? Polynomial(0) : intPow(primeBase, maxLevel)),
   m_base(primeBase),
   m_maxLevel(maxLevel)
{
   if ( !IterIrredTest(primeBase))
      throw std::invalid_argument("SizeParam: primeBase is not prime");

}

template<>
SizeParam<LatticeType::DIGITAL,PointSetType::MULTILEVEL>::SizeParam(uInteger primeBase, Level maxLevel):
   BasicSizeParam<SizeParam<LatticeType::DIGITAL,PointSetType::MULTILEVEL>>(primeBase == 0 ? 0 : intPow(primeBase, maxLevel)),
   m_base(primeBase),
   m_maxLevel(maxLevel)
{
   if (primeBase >= 2 and LatticeTester::IntFactor::isPrime(primeBase, 0) == LatticeTester::COMPOSITE)
      throw std::invalid_argument("SizeParam: primeBase is not prime");
}

//===================================================================================================================

template<>
SizeParam<LatticeType::ORDINARY,PointSetType::MULTILEVEL>::SizeParam(uInteger numPoints):
   BasicSizeParam<SizeParam<LatticeType::ORDINARY,PointSetType::MULTILEVEL>>(numPoints)
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
SizeParam<LatticeType::POLYNOMIAL,PointSetType::MULTILEVEL>::SizeParam(Polynomial modulus):
   BasicSizeParam<SizeParam<LatticeType::POLYNOMIAL,PointSetType::MULTILEVEL>>(modulus)
{
   if (IsZero(modulus)) {
      m_base = Polynomial(0);
      m_maxLevel = 0;
   }
   else {
      NTL::vector< NTL::Pair< Polynomial, long > > factors ;
      CanZass(factors, modulus); // calls "Cantor/Zassenhaus" algorithm from <NTL/GF2XFactoring.h>
      if (factors.size() != 1)
         throw std::runtime_error("not an integer power of a prime base");
      const auto& factor = *factors.begin();
      m_base = factor.a; // = factor.first
      m_maxLevel = factor.b; // = factor.second
   }
}

template<>
SizeParam<LatticeType::DIGITAL,PointSetType::MULTILEVEL>::SizeParam(uInteger numPoints):
   BasicSizeParam<SizeParam<LatticeType::DIGITAL,PointSetType::MULTILEVEL>>(numPoints)
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
SizeParam<LatticeType::ORDINARY,PointSetType::MULTILEVEL>::size_type
SizeParam<LatticeType::ORDINARY,PointSetType::MULTILEVEL>::numPointsOnLevel(Level level) const
{
   if (level > maxLevel())
      throw std::invalid_argument("level > maxLevel");
   return base() == 0 ? 0 : intPow(base(), level);
}

template<>
SizeParam<LatticeType::POLYNOMIAL,PointSetType::MULTILEVEL>::size_type
SizeParam<LatticeType::POLYNOMIAL,PointSetType::MULTILEVEL>::numPointsOnLevel(Level level) const
{
   if (level > maxLevel())
      throw std::invalid_argument("level > maxLevel");
   return IsZero(base())  ? 0 : intPow( 2, deg(base())*level );
}

template<>
SizeParam<LatticeType::DIGITAL,PointSetType::MULTILEVEL>::size_type
SizeParam<LatticeType::DIGITAL,PointSetType::MULTILEVEL>::numPointsOnLevel(Level level) const
{
   if (level > maxLevel())
      throw std::invalid_argument("level > maxLevel");
   return base() == 0 ? 0 : intPow(base(), level);
}
//========================================================================================================================

template<>
size_t
SizeParam<LatticeType::ORDINARY,PointSetType::MULTILEVEL>::totient() const
{ return base() == 0 ? 0 : (base() - 1) * this->numPoints() / base(); }

template<>
size_t
SizeParam<LatticeType::POLYNOMIAL,PointSetType::MULTILEVEL>::totient() const
{ return IsZero(base()) == 0 ? 0 : (intPow(2,deg(base())) - 1) * this->numPoints() / intPow(2,deg(base())); }

//=========================================================================================================================

template<LatticeType LR>
void
SizeParam<LR,PointSetType::MULTILEVEL>::normalize(Real& merit) const
{ merit /= this->numPoints(); }

template<LatticeType LR>
void
SizeParam<LR,PointSetType::MULTILEVEL>::normalize(RealVector& merit) const
{
   if (merit.size() != maxLevel() + 1)
      throw std::logic_error("merit vector size and maximum level do not match");
   for (Level level = 0; level < merit.size(); level++)
      merit[level] /= numPointsOnLevel(level);
}
//===========================================================================================================================
template<LatticeType LR>
std::ostream&
SizeParam<LR,PointSetType::MULTILEVEL>::format(std::ostream& os) const
{ os << base(); if (maxLevel() != 1) os << "^" << maxLevel(); return os; }
//===========================================================================================================================

template class SizeParam<LatticeType::ORDINARY,PointSetType::MULTILEVEL>;
template class SizeParam<LatticeType::POLYNOMIAL,PointSetType::MULTILEVEL>;
template class SizeParam<LatticeType::DIGITAL,PointSetType::MULTILEVEL>;

}

