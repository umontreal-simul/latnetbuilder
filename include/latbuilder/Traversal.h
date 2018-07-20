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

#ifndef LATBUILDER__TRAVERSAL_H
#define LATBUILDER__TRAVERSAL_H

#include <string>

#include "latbuilder/IndexedIterator.h"

namespace LatBuilder {
   
/**
 * Traversal types for indexable sequences.
 */
namespace Traversal {

/**
 * Range-based forward traversal type.
 */
class Forward {
public:
   /**
    * Size type.
    */
   typedef size_t size_type;

   static std::string name()
   { return "forward traversal"; }

   /**
    * Constructor.
    *
    * \param offset Index of the first element in the range.
    * \param size   Number of elements in the range.
    */
   Forward(size_type offset = 0, size_type size = std::numeric_limits<size_type>::max()):
      m_first(offset),
      m_last(offset + size)
   {}

   /**
    * Returns the traversal size.
    */
   size_t size() const
   { return m_last - m_first; }

   /**
    * Changes the traversal size to \c size.
    */
   void resize(size_type size)
   { m_last = m_first + size; }

protected:
   size_type m_first;
   size_type m_last;
};

/**
 * Random traversal type.
 */
template <typename RAND>
class Random {
public:
   /**
    * Size type.
    */
   typedef size_t size_type;

   /**
    * Type of the pseudo-random number generator.
    */
   typedef RAND RandomGenerator;

   static std::string name()
   { return "random traversal"; }

   /**
    * Constructor.
    *
    * \param rand       Random number generator.
    * \param size       Traversal size: number of sequence values to be
    *                   randomly visited.
    */
   Random(
         size_type size = std::numeric_limits<size_type>::max(),
         RandomGenerator rand = RandomGenerator()
         ):
      m_rand(std::move(rand)),
      m_size(size)
   {}

   /**
    * Returns the traversal size.
    */
   size_t size() const
   { return m_size; }

   /**
    * Changes the traversal size to \c size.
    */
   void resize(size_type size)
   { m_size = size; }

   /**
    * Returns the random generator.
    */
   const RandomGenerator& randomGenerator() const
   { return m_rand; }

   /**
    * Returns the random generator.
    */
   RandomGenerator& randomGenerator()
   { return m_rand; }

protected:
   RandomGenerator m_rand;
   size_type m_size;
};

/**
 * Traversal policy.  Must be specialized.
 */
template <typename SEQ, class TRAV>
class Policy;


/**
 * Traversal policy specialization for Forward traversal.
 */
template <typename SEQ>
class Policy<SEQ, Forward> : public Forward {
public:
   /**
    * Constructor.
    */
   explicit Policy(Forward trav):
      Forward(std::move(trav))
   {}

   /**
    * Immutable iterator type.
    */
   typedef IndexedIterator::Forward<SEQ> const_iterator;

   /**
    * Returns an iterator pointing to the selected element in \c seq.
    */
   const_iterator begin() const
   { return const_iterator(seq(), m_first); }

   /**
    * Returns an iterator pointing past the selected element in \c seq.
    */
   const_iterator end() const
   { return const_iterator(seq(), std::min(m_last, (size_type)seq().size())); }

private:
   const SEQ& seq() const
   { return static_cast<const SEQ&>(*this); }

};

/**
 * Traversal policy specialization for Random traversal.
 */
template <typename SEQ, typename RAND>
class Policy<SEQ, Random<RAND>> : public Random<RAND> {
public:
   /**
    * Constructor.
    */
   explicit Policy(Random<RAND> trav):
      Random<RAND>(std::move(trav))
   {}

   /**
    * Immutable iterator type.
    */
   typedef IndexedIterator::Random<SEQ, RAND> const_iterator;

   /**
    * Returns an iterator pointing to the first element in \c seq.
    */
   const_iterator begin() const
   { return const_iterator(seq(), this->randomGenerator()); }

   /**
    * Returns an iterator pointing past the last element in \c seq.
    */
   const_iterator end() const
   { return const_iterator(seq(), this->size()); }

   /**
    * Randomizes the traversal.  Iterators created after calling this
    * function will visit the sequence elements in a new random order.
    *
    * \remark The random generator must implement \c jump() for this to work.
    */
   void randomize()
   { this->m_rand.jump(); }

private:
   const SEQ& seq() const
   { return static_cast<const SEQ&>(*this); }

};

}}

#endif
