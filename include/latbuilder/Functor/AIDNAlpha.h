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

#ifndef LATBUILDER__FUNCTOR__AIDNALPHA_H
#define LATBUILDER__FUNCTOR__AIDNALPHA_H

#include "latbuilder/Types.h"

#include <cmath>
#include <sstream>

namespace LatBuilder { namespace Functor {

/**
 * One-dimensional merit function for the \f$\mathcal P_\alpha\f$ discrepancy. TODO
 *
 * This merit function is defined as
 * \f[
 *    \omega(x) =
 *    -\frac{(-4\pi^2)^{\alpha/2}}{\alpha!} \, B_\alpha(x),
 * \f]
 * for even integers \f$\alpha\f$, where \f$B_\alpha(x)\f$ is the Bernoulli
 * polynomial of degree \f$\alpha\f$.
 */
class AIDNAlpha {
public:
   typedef Real value_type;
   typedef Real result_type;

   /**
    * Constructor.
    *
    * \param alpha     Value of \f$\alpha\f$.
    */
   AIDNAlpha(unsigned int alpha, unsigned int interlacingFactor):
      m_alpha(alpha),
      m_interlacingFactor(interlacingFactor),
      m_min(std::min(m_alpha, m_interlacingFactor)),
      m_denom((sqrt( 1 << (m_alpha + 2)) * ( (1 << (m_min - 1)) -1 )))
   {}

   unsigned int alpha() const
   { return m_alpha; }

   unsigned int interlacingFactor() const
   { return m_interlacingFactor; }

   bool symmetric() const
   { return false; }

   static constexpr Compress suggestedCompression()
   { return Compress::NONE; }

   /**
    * Returns the one-dimensional function evaluated at \c x.
    */
   template<typename MODULUS>
   result_type operator()(const value_type& x, MODULUS n = 0) const
   { 
      if (x < std::numeric_limits<double>::epsilon()){
         return 1 / m_denom; 
      }
      else{
         return (1 - ( (double) (1 << m_min) - 1) / (1 << ( - (m_min - 1 ) * (int) std::floor(std::log2(x)) ))) / m_denom;
      }
   }

   std::string name() const
   { std::ostringstream os; os << "A-IDN" << alpha() << "-d" << interlacingFactor() ; return os.str(); }

private:
   unsigned int m_alpha;
   unsigned int m_interlacingFactor;
   unsigned int m_min;
   result_type m_denom;

};

/**
 * Formats \c functor and outputs it on \c os.
 */
inline
std::ostream& operator<<(std::ostream& os, const AIDNAlpha& functor)
{ return os << functor.name(); }

}}

#endif
