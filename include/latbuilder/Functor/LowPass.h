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

#ifndef LATBUILDER__FUNCTOR__LOW_PASS_H
#define LATBUILDER__FUNCTOR__LOW_PASS_H

namespace LatBuilder { namespace Functor {
/**
 * Low pass filter.
 */
template <typename T>
class LowPass {
public:
   /**
    * Constructor.
    * \param threshold Threshold value.
    */
   LowPass(T threshold = std::numeric_limits<T>::infinity()):
      m_threshold{std::move(threshold)}
   {}
   /**
    * Returns \c true if \c value is below the specified threshold.
    */
   bool operator()(const T& x) const
   { return x < m_threshold; }
   /**
    * Sets the threshold to \c threshold.
    */
   void setThreshold(T threshold)
   { m_threshold = std::move(threshold); }

   const T& threshold() const
   { return m_threshold; }

private:
   T m_threshold;
};

}}
#endif
