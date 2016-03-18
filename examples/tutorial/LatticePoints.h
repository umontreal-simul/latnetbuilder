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

#ifndef LATTICE_POINTS_H
#define LATTICE_POINTS_H

// virtual container for lattice points
class LatticePoints {
public:
   // standard container type definitions
   typedef std::vector<double> value_type;
   typedef size_t size_type;

   LatticePoints(size_type numPoints, std::vector<unsigned long> gen):
      m_numPoints(numPoints),
      m_intGen(std::move(gen)),
	  m_gen(m_intGen.size())
   { updateGen(); }

   // returns the number of points of the lattice rule
   size_type numPoints() const { return m_numPoints; }

   // returns the number of points of the lattice rule
   size_type size() const { return m_numPoints; }

   // returns the lattice dimension
   size_type dimension() const { return m_gen.size(); }

   // returns the i-th lattice point
   value_type operator[](size_type i) const
   {
      std::vector<double> point(dimension());
      for (size_type j = 0; j < point.size(); j++) {
         double x = i * m_gen[j];
         point[j] = x - int(x);
      }
      return point;
   }

private:
   size_type m_numPoints;
   std::vector<unsigned long> m_intGen;
   std::vector<double> m_gen;

   void updateGen()
   {
      for (size_type j = 0; j < m_gen.size(); j++)
         m_gen[j] = double(m_intGen[j]) / m_numPoints;
   }
};

#endif
