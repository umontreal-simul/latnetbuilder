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

#ifndef LATBUILDER__BRIDGE_ITERATOR_DYNAMIC_H
#define LATBUILDER__BRIDGE_ITERATOR_DYNAMIC_H

#include <boost/iterator/iterator_adaptor.hpp>
#include <limits>

namespace LatBuilder {

/**
 * Bridge iterator with dynamically computed value.
 */
template <typename SEQ>
class BridgeIteratorDynamic :
   public boost::iterator_adaptor<
      BridgeIteratorDynamic<SEQ>,
      typename SEQ::Base::const_iterator,
      const typename SEQ::value_type,
      boost::use_default,
      const typename SEQ::value_type
   >
{
   typedef BridgeIteratorDynamic<SEQ> self_type;

public:
   typedef typename SEQ::value_type value_type;
   typedef typename SEQ::size_type size_type;

   struct end_tag {};

   BridgeIteratorDynamic():
      self_type::iterator_adaptor_(),
      m_seq(nullptr)
   {}

   explicit BridgeIteratorDynamic(const SEQ& seq):
      self_type::iterator_adaptor_(seq.base().begin()),
      m_seq(&seq)
   {}

   BridgeIteratorDynamic(const SEQ& seq, end_tag):
      self_type::iterator_adaptor_(seq.base().end()),
      m_seq(&seq)
   {}

   const SEQ& seq() const
   { return *m_seq; }

   size_type index() const
   { return this->base_reference().index(); }

private:
   friend class boost::iterator_core_access;

   void increment()
   { ++this->base_reference(); }

   bool equal(const BridgeIteratorDynamic& other) const
   { return m_seq == other.m_seq and this->base_reference() == other.base_reference(); }

   value_type dereference() const
   { return m_seq->element(this->base_reference()); }

   ptrdiff_t distance_to(const BridgeIteratorDynamic& other) const
   { return m_seq == other.m_seq ? other.base_reference() - this->base_reference() : std::numeric_limits<ptrdiff_t>::max(); }

private:
   const SEQ* m_seq;
};

}

#endif
