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

#ifndef LATBUILDER__GENSEQ__GENERATING_VALUES_H
#define LATBUILDER__GENSEQ__GENERATING_VALUES_H

#include "latbuilder/Types.h"
#include "latbuilder/Util.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/CompressTraits.h"

#include <cstdlib>
#include <vector>
#include <limits>

namespace LatBuilder { namespace GenSeq {

/**
 * GeneratingValues traits.
 *
 * Specializations of this class template must define the types:
 * - size_type: an Integer size type;
 * - value_type: type of the elements coprime with the modulus ;
 * and must implement the static function:
 * - static std::string name()
 */
  template <class DERIVED>
    struct GeneratingValuesTraits;
/**
 * Indexed sequence of generating values: 
 * -For ordinary lattices: integers coprime with a specified modulus (Integer).
 * -For polynomial lattices: polynomials coprime with a specified modulus (Polynomial).
 */
  template <LatticeType,Compress = Compress::NONE, class TRAV = Traversal::Forward>
   class GeneratingValues ;
  
 


namespace detail {

    template <class Modulus>
   struct GeneratingValuesBasisElement {
    
    typedef size_t size_type;
    
      size_type totient;
      size_type leap;
      Modulus primeBase;
      Modulus elem;  
      GeneratingValuesBasisElement(size_type t = 0, size_type l = 0, Modulus p = (Modulus) (0), Modulus e = (Modulus) (0)):
         totient(t), leap(l), primeBase(p), elem(e)  {}
   };
}

}}

#include "latbuilder/GenSeq/GeneratingValues-OLR.h"
#include "latbuilder/GenSeq/GeneratingValues-PLR.h"

#endif
// vim: ft=cpp.doxygen
