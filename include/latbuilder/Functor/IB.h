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

#ifndef LATBUILDER__FUNCTOR__IB_H
#define LATBUILDER__FUNCTOR__IB_H

#include "latbuilder/Types.h"
#include "latbuilder/Util.h"

#include <cmath>
#include <sstream>

namespace LatBuilder { namespace Functor {

/**
 * One-dimensional merit function for the \f$\mathcal B_{d, \gamma, (2)}\f$ discrepancy in base 2.
 * This merit function equals \f$ \phi_{d, (2)} \f$ in \cite rGOD13a.
 *
 * This merit function is defined as
 * \f[
 *    \phi_{\d, (2)}(x) =
 *    \frac{2^{d-1}(1 - 2^{(d -1) \lfloor \log_2(x) \rfloor} (2^{d} -1))}{(2^{d - 1} -1) }
 * \f]
 * with \f$ \min(\alpha, d) > 1 \f$ where we set \f$2^{\lfloor \log_2(0) \rfloor} = 0\f$.
 */
class IB {
public:
   typedef Real value_type;
   typedef Real result_type;

   /**
    * Constructor.
    * \param interlacingFactor Value of \f$d\d$.
    */
   IB(unsigned int interlacingFactor):
      m_interlacingFactor(interlacingFactor),
      m_factor(intPow(2.0, m_interlacingFactor - 1) / (intPow(2.0, m_interlacingFactor - 1) - 1.0))
   {
        if (m_interlacingFactor < 2)
            throw std::runtime_error("Interlaced B kernel requires interlacing factor > 1");
   }

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
         return m_factor; 
      }
      else{
         return m_factor * (1.0 -  ( intPow(2.0, m_interlacingFactor) - 1.0) / intPow(2.0, - (m_interlacingFactor - 1 ) * (int) std::floor(std::log2(x)) ));
      }
   }

   std::string name() const
   { std::ostringstream os; os << "IB" << " - interlacing: " << interlacingFactor() ; return os.str(); }

private:
   unsigned int m_interlacingFactor;
   unsigned int m_min;
   result_type m_factor;

};

/**
 * Formats \c functor and outputs it on \c os.
 */
inline
std::ostream& operator<<(std::ostream& os, const IB& functor)
{ return os << functor.name(); }

}}

#endif
