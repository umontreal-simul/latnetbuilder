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

#ifndef LATTICE_POINTS_H
#define LATTICE_POINTS_H
#include "latbuilder/Types.h"
#include "latbuilder/Util.h"

// virtual container for lattice points
template<LatBuilder::LatticeType LA>
class LatticePoints {
public:
   // standard container type definitions
   typedef std::vector<double> value_type;
   typedef size_t size_type;
   typedef typename LatBuilder::LatticeTraits<LA>::Modulus Modulus;
   typedef typename LatBuilder::LatticeTraits<LA>::GeneratingVector GeneratingVector;

   LatticePoints(Modulus modulus, GeneratingVector gen):
      m_modulus(modulus),
      m_numPoints(LatBuilder::LatticeTraits<LA>::NumPoints(m_modulus)),
      m_gen(std::move(gen))
   { }

   // returns the modulus of the lattice
   Modulus modulus() const { return m_modulus; }

   // returns the number of points of the lattice rule
   size_type numPoints() const { return m_numPoints; }

   // returns the number of points of the lattice rule
   size_type size() const { return m_numPoints; }

   // returns the lattice dimension
   size_type dimension() const { return m_gen.size(); }

   // returns the i-th lattice point
   value_type operator[](size_type i) const ;

private:
   Modulus m_modulus;
   size_type m_numPoints;
   GeneratingVector m_gen;

};

//================================================================
template <>
auto LatticePoints<LatBuilder::LatticeType::ORDINARY>::operator[](size_type i) const -> value_type
{
   std::vector<double> point(dimension());
   for (size_type j = 0; j < point.size(); j++) {
       unsigned long x = (i * m_gen[j]) % numPoints();
       point[j] = (double)(x)/numPoints();
    }
    return point;
}

template <>
auto LatticePoints<LatBuilder::LatticeType::POLYNOMIAL>::operator[](size_type i) const -> value_type
{
   std::vector<double> point(dimension());
   for (size_type j = 0; j < point.size(); j++) {
       LatBuilder::Polynomial h = LatBuilder::PolynomialFromInt(i);
       unsigned long x = LatBuilder::Vm((h * m_gen[j]) % modulus(), modulus());
       point[j] = (double) (x) / numPoints();
    }
    return point;
}

#endif
