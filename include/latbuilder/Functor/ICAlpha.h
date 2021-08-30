// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#ifndef LATBUILDER__FUNCTOR__ICALPHA_H
#define LATBUILDER__FUNCTOR__ICALPHA_H

#include "latbuilder/Types.h"
#include "latbuilder/Util.h"

#include <cmath>
#include <sstream>

namespace LatBuilder { namespace Functor {

/**
 * One-dimensional merit function for the interlaced \f$\mathcal B_{\alpha, d, \gamma}\f$ discrepancy in base 2 \cite rGOD15c.
 * This merit function is defined as:
 * \f[
 *    \phi_{\alpha, d}(x) =
 *    \frac{1 - 2^{2 \min(\alpha, d) \lfloor \log_2(x) \rfloor} (2^{2 \min(\alpha, d) + 1} -1)}{2^{\alpha} (2^{2 \min(\alpha, d)} -1) }
 * \f]
 * with \f$ \min(\alpha, d) > 1 \f$ where we set \f$2^{\lfloor \log_2(0) \rfloor}
 = 0\f$.
 */
class ICAlpha {
public:
   typedef Real value_type;
   typedef Real result_type;

   /**
    * Constructor.
    *
    * \param alpha     Value of \f$\alpha\f$.
    * \param interlacingFactor Value of \f$d\f$.
    */
   ICAlpha(unsigned int alpha, unsigned int interlacingFactor):
      m_alpha(alpha),
      m_interlacingFactor(interlacingFactor),
      m_min(std::min(m_alpha, m_interlacingFactor)),
      m_denom(intPow(2.0, m_alpha) * (intPow(2.0, m_min) - 1.0 ))
    {
      if (m_interlacingFactor < 2)
        throw std::runtime_error("Interlaced C alpha kernel requires interlacing factor > 1");
    }

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
         return 1.0 / m_denom; 
      }
      else{
         return (1.0 - (intPow(2.0, 2 * m_min + 1) - 1.0) / intPow(2.0, - (2 * m_min) * (int) std::floor(std::log2(x)) ))   / m_denom;
      }
   }

   std::string name() const
   { std::ostringstream os; os << "IC - alpha: " << alpha() << " - interlacing: " << interlacingFactor() ; return os.str(); }

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
std::ostream& operator<<(std::ostream& os, const ICAlpha& functor)
{ return os << functor.name(); }

}}

#endif
