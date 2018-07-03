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

#ifndef LATBUILDER__INDEXED_ITERATOR_H
#define LATBUILDER__INDEXED_ITERATOR_H

#include <boost/iterator/iterator_facade.hpp>
#include <latbuilder/UniformUIntDistribution.h>

namespace LatBuilder {

/**
 * Immutable indexed iterators.
 */
namespace IndexedIterator {

/**
 * Constant forward indexed iterator.
 *
 * \tparam SEQ Type of sequence to which the iterator points.  Must implement
 *             value_type operator[](SEQ::size_type).
 */
template <typename SEQ>
class Forward : public boost::iterators::iterator_facade<
   Forward<SEQ>,
   const typename SEQ::value_type,
   boost::iterators::random_access_traversal_tag>
{
public:
   typedef SEQ Seq;
   typedef typename Seq::value_type value_type;
   typedef typename Seq::size_type size_type;

   Forward():
      Forward::iterator_facade_(),
      m_seq(nullptr),
      m_index(0)
   {}

   explicit Forward(const Seq& seq, size_type index = 0):
      Forward::iterator_facade_(),
      m_seq(&seq),
      m_index(index)
   { updateValue(); }

   /**
    * Returns the index of the element in the sequence this iterator is
    * currently pointing to.
    */
   size_type index() const
   { return m_index; }

   /**
    * Returns the index of the element in the sequence this iterator is
    * currently pointing to.
    */
   const Seq& seq() const
   { return *m_seq; }

private:
   friend class boost::iterators::iterator_core_access;

   void updateValue()
   { m_value = index() < seq().size() ? seq()[index()] : value_type(); }

   void increment()
   { ++m_index; updateValue(); }

   bool equal(const Forward& other) const
   { return m_seq == other.m_seq and index() == other.index(); }

   typename Forward::reference dereference() const
   { return m_value; }

   ptrdiff_t distance_to(const Forward& other) const
   { return m_seq == other.m_seq ? other.index() - index() : std::numeric_limits<ptrdiff_t>::max(); }

private:
   const Seq* m_seq;
   size_type m_index;
   value_type m_value;
};


/**
 * Immutable random indexed iterator.
 *
 * \tparam SEQ Type of sequence to which the iterator points.  Must implement
 *             value_type operator[](SEQ::size_type).
 * \tparam RAND     Random generator type.
 */
template <typename SEQ, typename RAND>
class Random : public boost::iterators::iterator_facade<
   Random<SEQ, RAND>,
   const typename SEQ::value_type,
   boost::iterators::forward_traversal_tag>
{
public:
   typedef SEQ Seq;
   typedef typename Seq::value_type value_type;
   typedef typename Seq::size_type size_type;
   typedef RAND RandomGenerator;

   struct end_tag {};

   explicit Random(
         const Seq& seq,
         RandomGenerator rand = RandomGenerator()):
      Random::iterator_facade_(),
      m_seq(&seq),
      m_count(0),
      m_index(0),
      m_unif(0, m_seq->size() - 1),
      m_rand(std::move(rand))
   { increment(); }

      // this constructor should not be used
   Random(const Seq& seq, size_type end):
      Random::iterator_facade_(),
      m_seq(&seq),
      m_count(end + 1),
      m_index(0)
   { }

      // this constructor should not be used
   Random():
      Random::iterator_facade_(),
      m_seq(nullptr),
      m_count(0),
      m_index(0)
   { }

   /**
    * Returns the index of the element in the sequence this iterator is
    * currently pointing to.
    */
   size_type index() const
   { return m_index; }

   /**
    * Returns a reference to the sequence.
    */
   const Seq& seq() const
   { return *m_seq; }

private:
   friend class boost::iterators::iterator_core_access;

   void updateValue()
   { m_value = index() < seq().size() ? seq()[index()] : value_type(); }

   void increment()
   { ++m_count; m_index = m_unif(m_rand); updateValue(); }

   bool equal(const Random& other) const
   { return m_seq == other.m_seq and m_count == other.m_count; }

   typename Random::reference dereference() const
   { return m_value; }

   ptrdiff_t distance_to(const Random& other) const
   { return m_seq == other.m_seq ? other.index() - index() : std::numeric_limits<ptrdiff_t>::max(); }
   //{ return m_seq == other.m_seq ? other.m_index - m_index : std::numeric_limits<ptrdiff_t>::max(); }

private:
   const Seq* m_seq;
   size_type m_count;
   size_type m_index;
   UniformUIntDistribution<size_type, RandomGenerator> m_unif;
   RandomGenerator m_rand;
   value_type m_value;
};

}}

#endif
