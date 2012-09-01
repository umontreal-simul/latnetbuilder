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
template <class TRAV = Traversal::Forward>
class Extend :
   public Traversal::Policy<Extend<TRAV>, TRAV> {

   typedef Extend<TRAV> self_type;
   typedef Traversal::Policy<self_type, TRAV> TraversalPolicy;

public:

   /**
    * Value type.
    */
   typedef Modulus value_type;

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
   Extend(value_type modulus = 1, value_type grain = 1, value_type low = 0, Traversal trav = Traversal());

   /**
    * Cross-traversal copy-constructor.
    */
   template <class TRAV2>
   Extend(
         const Extend<TRAV2>& other,
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
      typedef Extend<TRAV2> Type;
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
   template <typename> friend class Extend;

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

template <class TRAV>
Extend<TRAV>::Extend(
      value_type modulus,
          value_type grain,
          value_type low,
      Traversal trav):
   TraversalPolicy(std::move(trav)),
   m_modulus(modulus),
   m_grain(grain),
   m_low(low),
   m_size(modulus / grain)
{
}

template <class TRAV>
auto Extend<TRAV>::operator[](size_type i) const -> value_type
{
   return i * m_grain + m_low;
}

}}

#endif
