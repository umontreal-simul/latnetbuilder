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
