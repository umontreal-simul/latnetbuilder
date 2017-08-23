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

#ifndef LATBUILDER__SIZE_PARAM_EMBEDDED_H
#define LATBUILDER__SIZE_PARAM_EMBEDDED_H

#include "latbuilder/SizeParam.h"

namespace LatBuilder {

/**
 * Lattice size type for embedded lattices.
 * Defining parameters:
 * - prime base for the modulus (prime integer in the case of lattices rules and an irreductible polynomial in the case of polynomial lattices);
 * - maximum embedding level (power of the base for the last lattice in the sequence).
 */
template <Lattice LR>
class SizeParam<LR,LatType::EMBEDDED> :
   public BasicSizeParam<SizeParam<LR,LatType::EMBEDDED>> {

    typedef SizeParam<LR, LatType::ORDINARY> self_type;

public:

    typedef typename self_type::Modulus Modulus;
    typedef typename self_type::size_type size_type;

   /**
    * Constructor.
    *
    * \param primeBase     (prime integer in the case of lattices rules and an irreductible polynomial in the case of polynomial lattices).
    * \param maxLevel      The last lattice in the sequence (the one with the
    *                      largest number of points) has 
    *                      \f$\mathtt{primeBase}^{\mathtt{maxLevel}}\f$ as modulus.
    */
   SizeParam(Modulus primeBase, Level maxLevel);

   /**
    * Constructor.
    *
    * \param modulus  modulus factorizable as an integer power of
    *                   a prime base.
    */
   SizeParam(Modulus modulus = (Modulus)(0));

   template <class D>
   SizeParam(const BasicSizeParam<D>& other): SizeParam(other.modulus())
   {}

   /**
    * Returns the prime base for the modulus.
    */
   Modulus base() const
   { return m_base; }

   /**
    * Returns the maximum embedding level of the base for the last lattice in the sequence.
    */
   Level maxLevel() const
   { return m_maxLevel; }

   /**
    * Returns the number of points for the lattice at embedding level \c level
    * in the sequence.
    */
   size_type numPointsOnLevel(Level level) const;

   size_t totient() const;

   void normalize(Real& merit) const;

   /**
    * Divides the merit value at each level in \c merit by the number of
    * points at that level.
    */
   void normalize(RealVector& merit) const;

   std::ostream& format(std::ostream& os) const;

private:
   Modulus m_base;
   Level m_maxLevel;
};

}

#endif
