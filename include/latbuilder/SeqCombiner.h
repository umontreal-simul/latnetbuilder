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

#ifndef LATBUILDER__SEQ_COMBINER_H
#define LATBUILDER__SEQ_COMBINER_H

#include <boost/iterator/iterator_facade.hpp>

#include <vector>

namespace LatBuilder {

/**
 * Iterator incrementing policy that traverses unidimensional sequences
 * sequentially.
 */
template <typename DERIVED>
class CartesianProduct {
private:
   DERIVED& derived() { return static_cast<DERIVED&>(*this); }
public:
   bool increment()
   {
      // iterate backwards through components
      auto out = derived().m_value.rbegin();
      auto seq = derived().m_seq->seqs().rbegin();
      auto val = derived().m_its.rbegin();
      while (val != derived().m_its.rend()) {
         // if last value, reset iterator
         if (++*val == seq->end()) {
            *val = seq->begin();
            *out = **val;
         }
         // otherwise, use value
         else {
            *out = **val;
            return false;
         }
         ++out; ++seq; ++val;
      }
      return true;
   }
};

/**
 * Iterator incrementing policy that traverses unidimensional sequences in
 * parallel.
 */
template <typename DERIVED>
class Zip {
private:
   DERIVED& derived() { return static_cast<DERIVED&>(*this); }
public:
   bool increment()
   {
      // iterate backwards through components
      auto out = derived().m_value.rbegin();
      auto seq = derived().m_seq->seqs().rbegin();
      auto val = derived().m_its.rbegin();
      while (val != derived().m_its.rend()) {
         if (++*val == seq->end()) 
            return true;
         *out = **val;
         ++out; ++seq; ++val;
      }
      return false;
   }
};


/**
 * Multidimensional sequence composed of unidimensional sequences.
 *
 * \tparam SEQ       Type of unidimensional sequence.
 *
 * \tparam POLICY    Iterator incrementing policy.
 *                   If set to CartesianProduct, the output values of the
 *                   compound sequence are all values from the Cartesian product
 *                   of the input unidimensional sequences.
 *                   If set to Zip, the \f$i\f$-th output value is a
 *                   vector whose \f$j\f$-th component consist of the \f$i\f$-th
 *                   value of the \f$j\f$-th input sequence.
 */
template <
      typename SEQ,
      template <typename> class INCREMENT>
class SeqCombiner {
public:

   typedef SEQ Seq;
   typedef std::vector<typename Seq::value_type> value_type;
   typedef typename Seq::size_type size_type;

   /**
    * Constructor.
    *
    * \param seqs     Container of unidimenisonal sequences.
    */
   SeqCombiner(std::vector<Seq> seqs):
      m_seqs(std::move(seqs))
   {}

   /**
    * Returns the vector of unidimensional sequences.
    */
   const std::vector<Seq>& seqs() const
   { return m_seqs; }

private:
   std::vector<Seq> m_seqs;

public:

   /**
    * Constant iterator.
    */
   class const_iterator :
      public boost::iterators::iterator_facade<const_iterator,
      const value_type,
      boost::iterators::forward_traversal_tag>,
      private INCREMENT<const_iterator>
   {
   public:
      struct end_tag {};

      typedef std::vector<typename Seq::const_iterator> SeqIterators;

      explicit const_iterator(const SeqCombiner& seq):
         m_seq(&seq), m_at_end(m_seq->seqs().size() == 0)
      {
         // initial iterators and initial value
         m_value.resize(m_seq->seqs().size());
         m_its.resize(m_seq->seqs().size());
         auto val = m_value.begin();
         auto out = m_its.begin();
         auto in = m_seq->seqs().begin();
         while (out != m_its.end()) {
            *out = in->begin();
            *val = **out;
            ++in; ++out; ++val;
         }
      }

      const_iterator(const SeqCombiner& seq, end_tag):
         m_seq(&seq), m_at_end(true)
      { }

      /**
       * Returns a reference to the sequence.
       */
      const SeqCombiner& seq() const
      { return *m_seq; }

      /**
       * Returns a refenrence to all generator sequence iterators.
       */
      const SeqIterators& seqIterators() const
      { return m_its; }

   private:
      friend class boost::iterators::iterator_core_access;
      friend class INCREMENT<const_iterator>;

      bool equal(const const_iterator& other) const
      { 
         return m_seq == other.m_seq and (
               (m_at_end and other.m_at_end) or
               (m_its.size() == other.m_its.size()
                and std::equal(m_its.begin(), m_its.end(), other.m_its.begin())));
      }

      const value_type& dereference() const
      { return m_value;  }

      void increment()
      { m_at_end = INCREMENT<const_iterator>::increment(); }

   private:
      /**
       * Sequence.
       */
      const SeqCombiner* m_seq;
      /**
       * Flag indicating if the iterator is past the end of the sequence.
       */
      bool m_at_end;
      /**
       * Vector of current iterators on the generator sequence.
       */
      SeqIterators m_its;
      /**
       * Vector value.
       */
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

}

#endif
