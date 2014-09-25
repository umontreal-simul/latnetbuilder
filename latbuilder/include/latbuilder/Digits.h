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

#ifndef LATBUILDER__DIGITS_H
#define LATBUILDER__DIGITS_H

#include <vector>
#include <stdexcept>
#include <ostream>

namespace LatBuilder {

/**
 * Vector of digits representing a number in an arbitrary base.
 */
template <typename INT>
class Digits : public std::vector<unsigned int> {
public:
   typedef INT IntegerType;
   typedef typename Digits::value_type DigitType;
   typedef typename Digits::size_type size_type;

   /**
    * Constructor.
    *
    * \param base   The base of the digits.
    * \param number The initial number to be represented.
    */
   Digits(DigitType base, IntegerType number = IntegerType(0)):
      m_base(base)
   {
      if (number < 0)
         throw std::invalid_argument("number must be non-negative");

      while (number) {
         this->push_back(number % m_base);
         number /= m_base;
      }
   }

   /**
    * Returns the digits base.
    */
   const DigitType& base() const
   { return m_base; }

   /**
    * Computes and returns the number represented by the digits in this vector.
    */
   IntegerType value() const {
      IntegerType z = 0;
      for (auto it = this->rbegin(); it != this->rend(); ++it)
         z = z * base() + *it;
      return z;
   }

private:
   DigitType m_base;
};

template <typename INT>
std::ostream& operator<<(std::ostream& os, Digits<INT>& digits)
{
   os << digits.value() << " = (";
   auto it = digits.rbegin();
   if (it == digits.rend()) {
      os << 0;
   }
   else {
      os << *it;
      while (++it != digits.rend())
         os << " " << *it;
   }
   os << ")_{" << digits.base() << "}";
   return os;
}

}
#endif
