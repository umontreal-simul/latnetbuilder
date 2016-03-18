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

#ifndef LATBUILDER__BRIDGE_SEQ_H
#define LATBUILDER__BRIDGE_SEQ_H

#include "latbuilder/BridgeIteratorCached.h"

namespace LatBuilder {

/**
 * Bridge CRTP class for sequence based on another type of sequence, with
 * elements computed by the derived class.
 *
 * The only requirement on DERIVED is a function with the following signature:
 * \code
 * VALUE element(const typename BASE::const_iterator&) const;
 * \endcode
 *
 * \tparam DERIVED      Deriving class.
 * \tparam BASE         Base sequence class.
 * \tparam VALUE        Value type of the elements in the sequence.
 */
template <typename DERIVED,
         typename BASE,
         typename VALUE = typename BASE::value_type,
         template <typename> class ITERATOR = BridgeIteratorCached>
class BridgeSeq {
   typedef DERIVED Derived;

public:
   typedef BridgeSeq<DERIVED, BASE, VALUE, ITERATOR> BridgeSeq_;
   typedef BASE Base;
   typedef VALUE value_type;
   typedef typename Base::size_type size_type;

   /**
    * Constructor.
    *
    * \param base   Base sequence object.
    */
   BridgeSeq(Base base = Base()):
      m_base(std::move(base))
   {}

   /**
    * Returns the base sequence.
    */
   Base& base()
   { return m_base; }

   const Base& base() const
   { return m_base; }

   /**
    * Changes the base sequence to \c b.
    */
   void setBase(Base b)
   { m_base = std::move(b); }

private:
   Base m_base;

   value_type element(const typename Base::const_iterator& it) const
   { return static_cast<const Derived*>(this)->element(it); }

public:

   typedef ITERATOR<DERIVED> const_iterator;
   friend class ITERATOR<DERIVED>;

   /**
    * Returns an iterator pointing to the first element in the sequence.
    */
   const_iterator begin() const
   { return const_iterator(static_cast<const Derived&>(*this)); }

   /**
    * Returns an iterator pointing past the last element in the sequence.
    */
   const_iterator end() const
   { return const_iterator(static_cast<const Derived&>(*this), typename const_iterator::end_tag{}); }

};

}

#endif
