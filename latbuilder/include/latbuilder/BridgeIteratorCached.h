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

#ifndef LATBUILDER__BRIDGE_ITERATOR_CACHED_H
#define LATBUILDER__BRIDGE_ITERATOR_CACHED_H

#include <boost/iterator/iterator_adaptor.hpp>
#include <limits>
#include <stdexcept>

namespace LatBuilder {

/**
 * Bridge iterator with cached value.
 */
template <typename SEQ>
class BridgeIteratorCached :
   public boost::iterator_adaptor<
      BridgeIteratorCached<SEQ>,
      typename SEQ::Base::const_iterator,
      const typename SEQ::value_type
   >
{
   typedef BridgeIteratorCached<SEQ> self_type;

public:
   typedef typename SEQ::value_type value_type;
   typedef typename SEQ::size_type size_type;

   struct end_tag {};

   BridgeIteratorCached():
      self_type::iterator_adaptor_(),
      m_seq(nullptr)
   {}

   explicit BridgeIteratorCached(const SEQ& seq):
      self_type::iterator_adaptor_(seq.base().begin()),
      m_seq(&seq), m_cached(false)
   {}

   BridgeIteratorCached(const SEQ& seq, end_tag):
      self_type::iterator_adaptor_(seq.base().end()),
      m_seq(&seq), m_cached(false)
   {}

   const SEQ& seq() const
   { return *m_seq; }

   size_type index() const
   { return this->base_reference().index(); }

private:
   friend class boost::iterator_core_access;

   void increment()
   { ++this->base_reference(); m_cached = false; }

   bool equal(const BridgeIteratorCached& other) const
   { return m_seq == other.m_seq and this->base_reference() == other.base_reference(); }

   const value_type& dereference() const
   {
#ifndef NDEBUG
      if (this->base_reference() == m_seq->base().end())
         throw std::runtime_error("BridgeIteratorCached: dereferencing past end of sequence");
#endif
      if (!m_cached) {
         m_value = m_seq->element(this->base_reference());   
         m_cached = true;
      }
      return m_value;
   }

   ptrdiff_t distance_to(const BridgeIteratorCached& other) const
   { return m_seq == other.m_seq ? other.base_reference() - this->base_reference() : std::numeric_limits<ptrdiff_t>::max(); }

private:
   const SEQ* m_seq;
   mutable bool m_cached;
   mutable value_type m_value;
};

}

#endif
