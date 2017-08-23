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

#ifndef LATBUILDER__LAT_SEQ__CBC_H
#define LATBUILDER__LAT_SEQ__CBC_H

#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"

#include <boost/iterator/iterator_adaptor.hpp>

namespace LatBuilder { namespace LatSeq {

/**
 * Sequence of lattice definitions obtained by appending a variable component to
 * a base genrating vector.
 *
 * \tparam LAT       Type of lattice.
 * \tparam GENSEQ    Type of sequences of generator values.
 * 
 */
template <Lattice LR, LatType LAT, class GENSEQ>
class CBC {
public:

   typedef GENSEQ GenSeq;
   typedef LatDef<LR, LAT> value_type;
   typedef size_t size_type;

   /**
    * Constructor.
    *
    * \param baseLat    Base lattice.
    * \param genSeq     Sequence of generator sequences.
    */
   CBC(LatDef<LR, LAT> baseLat, GenSeq genSeq):
      m_baseLat(std::move(baseLat)), m_genSeq(std::move(genSeq))
   {}

   /**
    * Returns the sequences of generator values used for the last (appended
    * component) of the generating vector.
    */
   const GenSeq& genSeq() const
   { return m_genSeq; }

   /**
    * Returns the base lattice definition.
    */
   const LatDef<LR, LAT>& baseLat() const
   { return m_baseLat; }

private:
   LatDef<LR, LAT> m_baseLat;
   GenSeq m_genSeq;

public:

   /**
    * Constant iterator.
    */
   class const_iterator : public boost::iterators::iterator_adaptor<const_iterator,
      typename GenSeq::const_iterator,
      const value_type>
   {
   public:
      struct end_tag {};

      explicit const_iterator(const CBC& seq):
         const_iterator::iterator_adaptor_(seq.genSeq().begin()),
         m_seq(&seq),
         m_value(m_seq->baseLat())
      {
         if (this->base_reference() != m_seq->genSeq().end())
            m_value.gen().push_back(*this->base_reference());
      }

      const_iterator(const CBC& seq, end_tag):
         const_iterator::iterator_adaptor_(seq.genSeq().end()),
         m_seq(&seq)
      { }

      const CBC& seq() const
      { return *m_seq; }

   private:
      friend class boost::iterators::iterator_core_access;

      void increment()
      {
         const auto end = m_seq->genSeq().end();
         if (++this->base_reference() != end)
            m_value.gen().back() = *this->base_reference();
         else
            m_value.gen().back() = 0;
      }

      bool equal(const const_iterator& other) const
      { return m_seq == other.m_seq and this->base_reference() == other.base_reference(); }

      const value_type& dereference() const
      {
#ifndef NDEBUG
         if (this->base_reference() == m_seq->genSeq().end())
            throw std::runtime_error("LatSeq::CBC: dereferencing past end of sequence");
#endif
         return m_value;
      }

      ptrdiff_t distance_to(const const_iterator& other) const
      { return m_seq == other.m_seq ? other.base_reference() - this->base_reference() : std::numeric_limits<ptrdiff_t>::max(); }

   private:
      const CBC* m_seq;
      value_type m_value;
   };

   /**
    * Returns an iterator pointing to the first element in the sequence.
    */
   const_iterator begin() const
   { return const_iterator(*this); }

   /**
    * Returns an iterator pointing past the last element in the sequence.
    */
   const_iterator end() const
   { return const_iterator(*this, typename const_iterator::end_tag{}); }

};

/// Creates a CBC lattice sequence.
template <Lattice LR, LatType LAT, class GENSEQ>
CBC<LR, LAT, GENSEQ>
cbc(LatDef<LR, LAT> baseLat, GENSEQ genSeq)
{ return CBC<LR, LAT, GENSEQ>(std::move(baseLat), std::move(genSeq)); }

}}

#endif
