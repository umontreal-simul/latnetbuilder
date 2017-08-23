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

#ifndef LATBUILDER__EXTEND_H
#define LATBUILDER__EXTEND_H

#include "latbuilder/Util.h"
#include "latbuilder/Traversal.h"

#include <vector>
#include <limits>

namespace LatBuilder { namespace GenSeq {

/**
 * Indexed sequence of integers that extend on an smaller integer by adding
 * higher-order digits.
 *
 * Given a small integer \f$x\f$ smaller than the grain \f$n_0\f$, the
 * \f$i\f$-th integer this sequence is given by \f$i n_0 + x\f$.
 */
template <Lattice LR, class TRAV = Traversal::Forward>
class Extend :
   public Traversal::Policy<Extend<LR, TRAV>, TRAV> {

   typedef Extend<LR, TRAV> self_type;
   typedef Traversal::Policy<self_type, TRAV> TraversalPolicy;

public:

   /**
    * Value type.
    */
   typedef typename LatticeTraits<LR>::Modulus value_type;

   /**
    * Size type.
    */
   typedef size_t size_type;

   /**
    * Traversal type.
    */
   typedef TRAV Traversal;

   /**
    * Constructor.
    *
    * \param modulus    All integers in the sequence are smaller than \c modulus.
    * \param grain      Difference between successive integers in the sequence.
    * \param low  j     Integer smaller than \c grain that specifies the lower
    *                   digits of every integer in the sequence.
    * \param trav       Traversal instance.
    */
   Extend(value_type modulus = value_type(1), value_type grain = value_type(1), value_type low = value_type(0), Traversal trav = Traversal());

   /**
    * Cross-traversal copy-constructor.
    */
   template <class TRAV2>
   Extend(
         const Extend<LR, TRAV2>& other,
         Traversal trav = Traversal()):
      TraversalPolicy(std::move(trav)),
      m_modulus(other.m_modulus),
          m_grain(other.m_grain),
          m_low(other.m_low),
      m_size(other.m_size)
   {}

   /**
    * Rebinds the traversal type.
    */
   template <class TRAV2>
   struct RebindTraversal {
      typedef Extend<LR, TRAV2> Type;
   };

   /**
    * Returns a copy of this object, but using a different traversal policy.
    */
   template <class TRAV2>
   typename RebindTraversal<TRAV2>::Type rebind(TRAV2 trav) const
   { return typename RebindTraversal<TRAV2>::Type{*this, std::move(trav)}; }

   /**
    * Returns the modulus.
    */
   value_type modulus() const
   { return m_modulus; }

   /**
    * Returns the size of the sequence.
    *
    * The size is the value of Euler's totient function.
    */
   size_type size() const
   { return m_size; }

   /**
    * Returns the element at index \c i.
    */
   value_type operator[](size_type i) const;

private:
   template <Lattice, typename> friend class Extend;

   value_type m_modulus;
   value_type m_grain;
   value_type m_low;
   size_type m_size;
};

}}

//========================================================================
// IMPLEMENTATION
//========================================================================

namespace LatBuilder { namespace GenSeq {

template <Lattice LR, class TRAV>
Extend<LR, TRAV>::Extend(
      value_type modulus,
          value_type grain,
          value_type low,
      Traversal trav):
   TraversalPolicy(std::move(trav)),
   m_modulus(modulus),
   m_grain(grain),
   m_low(low),
   m_size(LatticeTraits<LR>::NumPoints(modulus / grain))
{
}

template <Lattice LR, class TRAV>
auto Extend<LR, TRAV>::operator[](size_type i) const -> value_type
{
   return LatticeTraits<LR>::ToGenValue(i) * m_grain + m_low;
}

}}

#endif
