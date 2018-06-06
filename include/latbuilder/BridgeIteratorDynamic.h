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

#ifndef LATBUILDER__BRIDGE_ITERATOR_DYNAMIC_H
#define LATBUILDER__BRIDGE_ITERATOR_DYNAMIC_H

#include <boost/iterator/iterator_adaptor.hpp>
#include <limits>
#include <stdexcept>

namespace LatBuilder {

/**
 * Bridge iterator with dynamically computed value.
 */
template <typename SEQ>
class BridgeIteratorDynamic :
   public boost::iterators::iterator_adaptor<
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
   friend class boost::iterators::iterator_core_access;

   void increment()
   { ++this->base_reference(); }

   bool equal(const BridgeIteratorDynamic& other) const
   { return m_seq == other.m_seq and this->base_reference() == other.base_reference(); }

   value_type dereference() const
   {
#ifndef NDEBUG
      if (this->base_reference() == m_seq->base().end())
         throw std::runtime_error("BridgeIteratorDynamic: dereferencing past end of sequence");
#endif
      return m_seq->element(this->base_reference());
   }

   ptrdiff_t distance_to(const BridgeIteratorDynamic& other) const
   { return m_seq == other.m_seq ? other.base_reference() - this->base_reference() : std::numeric_limits<ptrdiff_t>::max(); }

private:
   const SEQ* m_seq;
};

}

#endif
