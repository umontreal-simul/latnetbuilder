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

#ifndef LATBUILDER__GENSEQ__CYCLIC_GROUP_H
#define LATBUILDER__GENSEQ__CYCLIC_GROUP_H

#include "latbuilder/Traversal.h"
#include "latbuilder/CompressTraits.h"

#include <iterator>
#include <boost/iterator/iterator_facade.hpp>


namespace LatBuilder { namespace GenSeq {

/// Group ordering type.
enum class GroupOrder : bool { DIRECT = false, INVERSE = true };
/// Returns the inverse group order.
constexpr GroupOrder operator!(GroupOrder order) { return order == GroupOrder::DIRECT ? GroupOrder::INVERSE : GroupOrder::DIRECT; }

/**
 * Cyclic group traits.
 *
 * Specializations of this class template must define the types:
 * - size_type: an Integer size type;
 * - Modulus: Modulus type;
 * - value_type: type of the elements of the group ;
 * - and the static function void increment(const value_type& currentValue, const value_type& generator, const Modulus& modulus)
 */
  template <class DERIVED>
    struct CyclicGroupTraversalTraits;
/**
 * Default traversal policy for cyclic groups.
 */
template<LatticeType LR>
class CyclicGroupTraversal {
public:
   typedef CyclicGroupTraversal<LR> self_type;
   typedef typename CyclicGroupTraversalTraits<self_type>::size_type size_type;
   typedef typename CyclicGroupTraversalTraits<self_type>::value_type value_type;
   typedef typename CyclicGroupTraversalTraits<self_type>::Modulus Modulus;

   static std::string name()
   { return "cyclic group traversal"; }

   CyclicGroupTraversal():
      m_size(std::numeric_limits<size_t>::max())
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
    * increments to  next value.
    */
   static void increment(value_type& currentValue, size_type& index, const value_type& generator, const Modulus& modulus, const size_type& size){
     CyclicGroupTraversalTraits<self_type>::increment(currentValue,index,generator,modulus,size);
   }

private:
   size_type m_size;
};

/**
 * Cyclic group traits.
 *
 * Specializations of this class template must define the types:
 * - size_type: an Integer size type;
 * - Modulus: Modulus type;
 * - value_type: type of the elements of the group ;
 */
  template <class DERIVED>
    struct CyclicGroupTraits;

/**
 * Cyclic group.
 *
 * Cyclic group \f$\{ 1, g, g^2, \dots, g^{\varphi(b^m) - 1} \}\f$ of integers
 * modulo \f$b^m\f$ (respectively polynomials modulo \f$b^m\f$), where \f$b\f$ is a prime base (respectively an irreductible polynomial), \f$g\f$ is the generator of
 * the group, and \f$\varphi\f$ is Euler's totient function.
 * 
 * The generator of the group is computed by the class constructor.
 *
 * \tparam COMPRESS  Compression type.  If Compress::SYMMETRIC, only the first
 *                   half of the group is considered and an element value of
 *                   \f$a\f$ is mapped to \f$\min(a, b^m - a)\f$. (available only in the integer case)
 *
 * \remark 
 * -For the integer case, and in base 2, the group is the union of two cyclic groups rather than a
 *      single cyclic group.
 * - In the polynomial case, the power must be set to 1 otherwise the group does not have a cyclic stucture
 */
template <LatticeType LR, 
         Compress COMPRESS = Compress::NONE,
         class TRAV = CyclicGroupTraversal<LR>,
         GroupOrder ORDER = GroupOrder::DIRECT>
class CyclicGroup ;




}} // namespace

#include "latbuilder/GenSeq/CyclicGroup-OLR.h"
#include "latbuilder/GenSeq/CyclicGroup-PLR.h"

namespace LatBuilder { namespace Traversal {
   template <typename SEQ, LatticeType LR>
   class Policy<SEQ, GenSeq::CyclicGroupTraversal<LR>> :
      public GenSeq::CyclicGroupTraversal<LR> {

   public:
      typedef SEQ Seq;
      typedef typename GenSeq::CyclicGroupTraversal<LR>::value_type value_type;
      typedef typename GenSeq::CyclicGroupTraversal<LR>::size_type size_type;

      /**
       * Constructor.
       */
      Policy(GenSeq::CyclicGroupTraversal<LR> trav):
         GenSeq::CyclicGroupTraversal<LR>(std::move(trav))
      {}

      /**
       * Immutable iterator type.
       */
      class const_iterator : public boost::iterators::iterator_facade<const_iterator,
         const value_type,             // value
         boost::iterators::random_access_traversal_tag, // traversal
         const value_type,             // reference
         ptrdiff_t>                    // difference
      {
      public:
         struct end_tag {};

         explicit const_iterator(const Seq& seq, size_type index = 0):
            m_seq(&seq), m_index(index), m_value(1)
         {}

         const_iterator():
            m_seq(nullptr), m_index(0), m_value(0)
         {}

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

         value_type dereference() const   
         { return m_value == (value_type)(1) ?  (value_type)(1) : CompressTraits<Seq::compress()>::compressIndex(m_value, m_seq->modulus()); }

         void increment()
         {
            
            GenSeq::CyclicGroupTraversal<LR>::increment(m_value, m_index, m_seq->generator(), m_seq->modulus(),m_seq->size());
           

         }

         bool equal(const const_iterator& other) const
         { return m_seq == other.m_seq and m_index == other.m_index; }

         ptrdiff_t distance_to(const const_iterator& other) const
         { return m_seq == other.m_seq ? other.m_index - m_index : std::numeric_limits<ptrdiff_t>::max(); }

      private:
         const Seq* m_seq;
         size_type m_index;
         value_type m_value;
      };

      /**
       * Returns an iterator pointing to the first element in \c seq.
       */
      const_iterator begin() const
      { return const_iterator(seq(), 0); }

      /**
       * Returns an iterator pointing past the last element in \c seq.
       */
      const_iterator end() const
      { return const_iterator(seq(), std::min(this->size(), (size_type)seq().size())); }

   private:
      const Seq& seq() const
      { return static_cast<const Seq&>(*this); }
   };
}}

#endif
