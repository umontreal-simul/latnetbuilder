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
class Digits : public std::vector<INT> {
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
