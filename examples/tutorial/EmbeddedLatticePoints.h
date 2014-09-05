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

#ifndef EMBEDDED_LATTICE_POINTS_H
#define EMBEDDED_LATTICE_POINTS_H

// virtual container for lattice points
class EmbeddedLatticePoints {
public:
   // standard container type definitions
   typedef std::vector<double> value_type;
   typedef size_t size_type;

   EmbeddedLatticePoints(size_type base, size_type maxLevel, std::vector<unsigned long> gen):
      m_base(base),
      m_maxLevel(maxLevel),
      m_intGen(std::move(gen)),
      m_gen(m_intGen.size())
   { reset(); }

   // returns to level 0
   void reset() { m_level = 0; m_numPoints = 1; updateGen(); }

   // increases the level
   void extend() { m_level++; m_numPoints *= m_base; updateGen(); }

   // returns the base for the number of points
   size_type base() const { return m_base; }

   // returns the maximum level number
   size_type maxLevel() const { return m_maxLevel; }

   // returns the current level number
   size_type level() const { return m_level; }

   // returns the total number of points on the current level
   size_type numPoints() const { return m_numPoints; }

   // returns the number of points on the current level but not on previous
   // levels
   size_type size() const { return numPoints() <= 1 ? numPoints() : (base() - 1) * numPoints() / base(); }

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
   size_type m_base;
   size_type m_maxLevel;
   std::vector<unsigned long> m_intGen;
   std::vector<double> m_gen;
   size_type m_numPoints;
   size_type m_level;

   size_type map(size_type i) const { return numPoints() <= 1 ? i : (base() + 1) * (i + 1) / base(); }

   void updateGen()
   {
      for (size_type j = 0; j < m_gen.size(); j++)
         m_gen[j] = double(m_intGen[j]) / m_numPoints;
   }
};

#endif
