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

#ifndef LATBUILDER__SIZE_PARAM_SIMPLE_H
#define LATBUILDER__SIZE_PARAM_SIMPLE_H

#include "latbuilder/SizeParam.h"

namespace LatBuilder {

/**
 * Ordinary lattice size parameter.
 */
template <LatticeType LR>
class SizeParam<LR, EmbeddingType::UNILEVEL> :
   public BasicSizeParam<SizeParam<LR, EmbeddingType::UNILEVEL>> {

    typedef SizeParam<LR, EmbeddingType::UNILEVEL> self_type;

public:

   typedef typename self_type::Modulus Modulus;
   typedef typename self_type::size_type size_type;

   SizeParam(Modulus modulus = (Modulus)(0));

   template <EmbeddingType L>
   SizeParam(const SizeParam<LR,L>& other): SizeParam(other.modulus())
   {}

   /**
    * Returns the value of Euler's totient function.
    * For ordinary lattices It is the number of positive integers that are smaller than and coprime
    * to the number of points.
    * For polynomial lattices It is the number of polynomials that have smaller degree than and coprime
    * to the modulus polynomial.
    */
   size_t totient() const;

   /**
    * Divides the merit value \c merit by the number of points.
    */
   void normalize(Real& merit) const;

   /**
    * Divides the each merit value in \c merit by the number of points.
    */
   void normalize(RealVector& merit) const;

   std::ostream& format(std::ostream& os) const;
};


}

#endif
