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
