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

#ifndef LATBUILDER__FUNCTOR__PALPHA_TILDE_H
#define LATBUILDER__FUNCTOR__PALPHA_TILDE_H


#include "latbuilder/Types.h"


#include <cmath>
#include <sstream>

namespace LatBuilder { namespace Functor {

/**
 * One-dimensional merit function for the \f$\tilde{\mathcal{P}}_{\alpha}\f$ discrepancy.
 *
 * This merit function is defined as
 * \f[
 *    \omega(x) = \mu(\alpha)  - 2^{(1+ \lfloor \log_{2}(x_{i,j}) \rfloor)(\alpha-1)}(\mu(\alpha)+1)
 * \f]
 * for \f$\alpha >1\f$ 
 */
class PAlphaTilde {
public:
   typedef Real value_type;
   typedef Real result_type;

   /**
    * Constructor.
    *
    * \param alpha     Value of \f$\alpha\f$.
    */
   PAlphaTilde(unsigned int alpha):
      m_alpha(alpha)
   {
      m_mu = 1/ (1 - std::pow( (Real)(2),  (1 - (Real) (m_alpha)) )) ;
   }

   unsigned int alpha() const
   { return m_alpha; }

   bool symmetric() const
   { return false; }

   static constexpr Compress suggestedCompression()
   { return Compress::NONE; }

   /**
    * Returns the one-dimensional function evaluated at \c x.
    */
   result_type operator()(const value_type& x, Polynomial n = Polynomial(0)) const
   { return m_mu - std::pow(  (Real)(2), (1+ std::floor(std::log2(x)))*(m_alpha-1)  )*( 1 + m_mu) ;}

   result_type operator()(const value_type& x, unsigned long) const
   { return m_mu - std::pow(  (Real)(2), (1+ std::floor(std::log2(x)))*(m_alpha-1)  )*( 1 + m_mu) ;}

   std::string name() const
   { std::ostringstream os; os << "P" << alpha() << "_PLR"; return os.str(); }

private:
   unsigned int m_alpha;
   Real m_mu;
   
};

/**
 * Formats \c functor and outputs it on \c os.
 */
inline
std::ostream& operator<<(std::ostream& os, const PAlphaTilde& functor)
{ return os << functor.name(); }

}}

#endif
