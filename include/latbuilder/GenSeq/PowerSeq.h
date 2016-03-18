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

#ifndef LATBUILDER__GEN_SEQ__POWER_SEQ_H
#define LATBUILDER__GEN_SEQ__POWER_SEQ_H

#include "latbuilder/Types.h"
#include "latbuilder/BridgeSeq.h"
#include "latbuilder/Util.h"

namespace LatBuilder { namespace GenSeq {

/**
 * Sequence filter that raises values to a given power, optionally modulo a
 * given integer.
 */
template <typename BASE>
class PowerSeq :
   public BridgeSeq<
      PowerSeq<BASE>,
      BASE,
      typename BASE::value_type,
      BridgeIteratorCached>
{
   typedef PowerSeq<BASE> self_type;

public:

   typedef typename self_type::Base Base;
   typedef typename self_type::value_type value_type;
   typedef typename self_type::size_type size_type;
   typedef typename BASE::Traversal Traversal;

   /**
    * Constructor.
    *
    * \param base Base sequence.
    * \param power Power to which the elements of the sequence will be raised.
    * \param modulus If nonzero, integer against which to apply the modulo.
    */
   PowerSeq(Base base = Base(), unsigned int power = 1, Modulus modulus = 0):
      self_type::BridgeSeq_(std::move(base)),
      m_power(power),
      m_modulus(modulus)
   {}

   /**
    * Cross-traversal copy-constructor.
    */
   template <typename BASE2>
   PowerSeq(
         const PowerSeq<BASE2>& other,
         typename Base::Traversal trav = typename Base::Traversal()):
      self_type::BridgeSeq_(other.base().rebind(trav)),
      m_power(other.m_power),
      m_modulus(other.m_modulus)
   {}

   /**
    * Rebinds the traversal type.
    */
   template <class TRAV2>
   struct RebindTraversal {
      typedef PowerSeq<typename Base::template RebindTraversal<TRAV2>::Type> Type;
   };

   /**
    * Returns a copy of this object, but using a different traversal policy.
    */
   template <class TRAV2>
   typename RebindTraversal<TRAV2>::Type rebind(TRAV2 trav) const
   { return typename RebindTraversal<TRAV2>::Type{*this, std::move(trav)}; }

   /**
    * Computes and returns the output value of the filters for the basic merit
    * value pointed to by \c it.
    */
   value_type element(const typename Base::const_iterator& it) const
   { return modulus() ? modularPow(*it, power(), modulus()) : intPow(*it, power()); }

   /**
    * Returns the size of the underlying sequence.
    */
   size_type size () const
   { return this->base().size(); }

   /**
    * Calls resize() on the base sequence.
    */
   void resize(size_type size)
   { this->base().resize(size); }

   Modulus modulus() const
   { return m_modulus; }

   unsigned int power() const
   { return m_power; }

private:
   template <typename> friend class PowerSeq;
   unsigned int m_power;
   Modulus m_modulus;
};

}}

#endif
