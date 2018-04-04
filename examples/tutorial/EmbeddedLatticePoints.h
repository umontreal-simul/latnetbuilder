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

#ifndef EMBEDDED_LATTICE_POINTS_H
#define EMBEDDED_LATTICE_POINTS_H
#include "latbuilder/Types.h"
#include "latbuilder/Util.h"
// virtual container for lattice points
template<LatBuilder::LatticeType LA>
class EmbeddedLatticePoints {
public:
   // standard container type definitions
   typedef std::vector<double> value_type;
   typedef size_t size_type;
   typedef typename LatBuilder::LatticeTraits<LA>::Modulus Modulus;
   typedef typename LatBuilder::LatticeTraits<LA>::GeneratingVector GeneratingVector;

   EmbeddedLatticePoints(Modulus base, size_type maxLevel, GeneratingVector gen):
      m_base(base),
      m_maxLevel(maxLevel),
      m_gen(std::move(gen))
   { reset(); }

   // returns to level 0
   void reset() { m_level = 0; m_numPoints = 1;  m_modulus = Modulus(1); }

   // increases the level
   void extend() { m_level++; m_numPoints *= LatBuilder::LatticeTraits<LA>::NumPoints(m_base); m_modulus *= m_base; }

   // returns the base for the number of points
   Modulus base() const { return m_base; }

   // returns the maximum level number
   size_type maxLevel() const { return m_maxLevel; }

   // returns the current level number
   size_type level() const { return m_level; }

   // returns the total number of points on the current level
   size_type numPoints() const { return m_numPoints; }

   // returns the modulus of the current level
   Modulus modulus() const { return m_modulus; }

   // returns the number of points on the current level but not on previous
   // levels
   size_type size() const { return numPoints() <= 1 ? numPoints() : (LatBuilder::LatticeTraits<LA>::NumPoints(m_base) - 1) * numPoints() / LatBuilder::LatticeTraits<LA>::NumPoints(m_base); }

   // returns the lattice dimension
   size_type dimension() const { return m_gen.size(); }

   // returns the i-th lattice point that lies on the current levels but not on
   // previous levels
   value_type operator[](size_type i) const
   {
      std::vector<double> point(dimension());
      for (size_type j = 0; j < point.size(); j++) {
         double x = map(i) * m_gen[j];
         point[j] = x - int(x);
      }
      return point;
   }

private:
   Modulus m_base;
   size_type m_maxLevel;
   GeneratingVector m_gen;
   size_type m_numPoints;
   size_type m_level;
   Modulus m_modulus;

   size_type map(size_type i) const { return numPoints() <= 1 ? i : (base() + 1) * (i + 1) / base(); }

};

//================================================================
template <>
auto EmbeddedLatticePoints<LatBuilder::LatticeType::ORDINARY>::operator[](size_type i) const -> value_type
{
   std::vector<double> point(dimension());
   for (size_type j = 0; j < point.size(); j++) {
       unsigned long index = numPoints() <= 1 ? i : (base() + 1) * (i + 1) / base();
       point[j] = (double)(index * m_gen[j] % numPoints())/numPoints();
    }
    return point;
}

template <>
auto EmbeddedLatticePoints<LatBuilder::LatticeType::POLYNOMIAL>::operator[](size_type i) const -> value_type
{
   std::vector<double> point(dimension());
   const unsigned long leap = LatBuilder::LatticeTraits<LatBuilder::LatticeType::POLYNOMIAL>::NumPoints(base()) - 1;
   for (size_type j = 0; j < point.size(); j++) {
       unsigned long R = i % leap +1;
       unsigned long Q = i / leap ;
       LatBuilder::Polynomial h = numPoints() <= 1 ? LatBuilder::PolynomialFromInt(i) : LatBuilder::PolynomialFromInt(Q) * base() + LatBuilder::PolynomialFromInt(R);
       unsigned long x = LatBuilder::Vm((h * m_gen[j]) % modulus(), modulus());
       point[j] = (double) (x) / numPoints();
    }
    return point;
}

#endif