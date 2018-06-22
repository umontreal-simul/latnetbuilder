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

#ifndef LATBUILDER__STORAGE_EMBEDDED_H
#define LATBUILDER__STORAGE_EMBEDDED_H

#include "latbuilder/SizeParam.h"
#include "latbuilder/GenSeq/CyclicGroup.h"
#include "latbuilder/GenSeq/GeneratingValues.h"

#include <map>
#include <stdexcept>

namespace LatBuilder {

template <PerLevelOrder, LatticeType LR, Compress COMPRESS>
struct PerLevelOrderTraits;

template <LatticeType LR, Compress COMPRESS>
struct PerLevelOrderTraits<PerLevelOrder::CYCLIC, LR, COMPRESS>{
  typedef GenSeq::CyclicGroup<LR, COMPRESS, Traversal::Forward, GenSeq::GroupOrder::INVERSE> GroupType; 
  typedef GenSeq::CyclicGroup<LR, COMPRESS, Traversal::Forward, GenSeq::GroupOrder::DIRECT> GenGroupType; 
  typedef typename GroupType::value_type  value_type;
  static void initializeGroups(const value_type& base, const Level& maxLevel, std::vector<GroupType>& groups, GenGroupType& genGroup){
        genGroup = GenGroupType(base, maxLevel) ;
        // initialize groups
        GroupType maxLevelGroup(base, maxLevel);
        for (Level level = 0; level <= maxLevel; level++)
             groups[level] = maxLevelGroup.subgroup(level);
  }

};

template <LatticeType LR, Compress COMPRESS>
struct PerLevelOrderTraits<PerLevelOrder::BASIC, LR, COMPRESS>{
  typedef GenSeq::GeneratingValues<LR, COMPRESS, Traversal::Forward> GroupType; 
  typedef GroupType GenGroupType; 
  typedef typename GroupType::value_type  value_type;
  static void initializeGroups(const value_type& base, const Level& maxLevel, std::vector<GroupType>& groups, GenGroupType& genGroup){
      
      genGroup = GenGroupType(intPow(base, maxLevel)) ;
      auto mult = typename LatticeTraits<LR>::Modulus(1);
      for (Level level = 0; level <= maxLevel; level++){
          groups[level] = GroupType(mult);
          mult *=  base ;
       }
  }
  
};

template <Compress COMPRESS>
struct PerLevelOrderTraits<PerLevelOrder::BASIC, LatticeType::DIGITAL, COMPRESS>{
  typedef GenSeq::GeneratingValues<LatticeType::ORDINARY, COMPRESS, Traversal::Forward> GroupType; 
  typedef GroupType GenGroupType; 
  typedef typename GroupType::value_type  value_type;
  static void initializeGroups(const value_type& base, const Level& maxLevel, std::vector<GroupType>& groups, GenGroupType& genGroup){
      
      genGroup = GenGroupType(intPow(base, maxLevel)) ;
      auto mult = typename LatticeTraits<LatticeType::DIGITAL>::Modulus(1);
      for (Level level = 0; level <= maxLevel; level++){
          groups[level] = GroupType(mult);
          mult *=  base ;
       }
  }
  
};


template <LatticeType LR, Compress COMPRESS, PerLevelOrder PLO>
struct StorageTraits<Storage<LR, EmbeddingType::MULTILEVEL, COMPRESS, PLO>> {
   typedef uInteger               size_type;
   typedef CompressTraits<COMPRESS> Compress;
   typedef RealVector            MeritValue;
   typedef LatBuilder::SizeParam<LR, EmbeddingType::MULTILEVEL> SizeParam;

   typedef typename PerLevelOrderTraits<PLO, LR, COMPRESS>::GroupType GroupType; 
   typedef typename PerLevelOrderTraits<PLO, LR, COMPRESS>::GenGroupType GenGroupType; 

   typedef typename GroupType::value_type  value_type;

   /**
    * Unpermuted permutation.
    */
   class Unpermute {
   public:
      typedef StorageTraits::size_type size_type;
      typedef std::pair<boost::numeric::ublas::range, size_type> AddressType;
      typedef StorageTraits::value_type value_type;
      Unpermute(Storage<LR, EmbeddingType::MULTILEVEL, COMPRESS, PLO> storage):
         m_storage(std::move(storage))
      {
         // initialize addresses
         m_indexToAddress[value_type(0)] = std::make_pair(boost::numeric::ublas::range{0, 1}, 0);
         size_t start = 1;
         typename GroupType::value_type mult = m_storage.sizeParam().modulus();
         for (Level level = 1; level <= m_storage.sizeParam().maxLevel(); level++) {
            mult /= m_storage.sizeParam().base();
            const auto& subgroup = m_storage.indices(level);
            typename GroupType::size_type i = 0;
            boost::numeric::ublas::range range{start, start + subgroup.size()};
            for (const auto g : subgroup)
               m_indexToAddress[mult * g] = std::make_pair(range, i++);
            start += subgroup.size();
         }
      }

      size_type operator() (size_type i) const
      {
         const auto where = indexToAddress( LatticeTraits<LR>::ToGenValue(i));
         return where.first.start() + where.second;
      }

      size_type size() const
      { return m_storage.virtualSize(); }

   private:
      typedef std::map< value_type, AddressType> MapType;

      Storage<LR, EmbeddingType::MULTILEVEL, COMPRESS, PLO > m_storage;
      MapType m_indexToAddress;

      /**
       * Returns the address of the element at index \c i.
       *
       * The address is a pair of which the first element is a range on the
       * elements indices for the level, and the sub-index on that level.
       */
      const AddressType& indexToAddress(value_type i) const
      {
         i = Compress::compressIndex(i, m_storage.sizeParam().modulus());
         typename MapType::const_iterator it = m_indexToAddress.find(i);
         if (it == m_indexToAddress.end())
            throw std::out_of_range("index is too large");
         return it->second;
#if 0
         using boost::numeric::ublas::range;

         if (i == 0)
            return std::pair<range, size_t>{{0, 1}, 0};

         if (i >= m_storage.sizeParam().numPoints())
            throw std::out_of_range("index is too large");

         const uInteger base = m_storage.sizeParam().base();

         i = Compress::compressIndex(i, m_storage.sizeParam().numPoints());

         size_t end = m_storage.sizeParam().numPoints();
         Level level = m_storage.sizeParam().maxLevel();
         while (i % base == 0) {
            i /= base;
            end /= base;
            level--;
         }

         // level start
         const size_t begin = end / base;

         // permute on-level index
         i = m_indexToAddress[level].indexToAddress(i)->second;

         //GenSeq::CyclicGroup<Compress, GenSeq::GroupOrder::INVERSE> indices(base, level);
         //i = group.find(i) - group.begin();

         return std::pair<range, size_t>{{Compress::size(begin), Compress::size(end)}, i};
#endif
      }
   };

   /**
    * Stride permutation.
    *
    * The elements on each block are visited by applying a stride permutaion.
    * The \f$k^{\text{th}}\f$ level is the concatenation of the first \f$k\f$ block. (more details in Storage)
    *
    * 
    *
    * For ordinary lattices: 
    * a Stride with parameter \f$a \in \mathbb{N}\f$ is the mapping that maps an index \f$i\f$ to \f$a \times i \mod n\f$.\n
    * Consider the unpermuted vector \f$\boldsymbol v = (v_1, \dots, v_n)\f$ for
    * some positive integer \f$n\f$.  The \f$j\f$-th component of the vector
    * with stride length \f$a\f$ has value \f$v_{j a \bmod n}\f$.
    *
    * 
    *
    * For polynomial lattices: 
    * A Stride with parameter \f$q(z) \in \mathbb{Z}_2[z]\f$ is the mapping that maps a polynomial \f$i(z)\f$ modulo \f$P(z)\f$ with the polynomial \f$h(z) = i(z)q(z) \mod P(z)\f$ .\n
    * Consider the unpermuted vector \f$\boldsymbol v = (v_1, \dots, v_n) \sim (v_{1(z)}, \dots, v_{n(z)})\f$ for
    * some positive integer \f$n\f$, where \f$j(z) = \sum a_lz^l\f$ if \f$j = \sum a_l2^l\f$. Let \f$P(z)\f$ be the modulus polynomial.  The \f$j\f$-th component of the vector
    * with stride parameter \f$q(z) \in \mathbb{Z}_2[z]\f$ has value \f$v_{j(z) q(z) \bmod P(z)}\f$.
    *
    *
    * 
    *
    * If the per level order is inverse-cyclic: 
    * The stride length \f$a\f$ can be identified to the \f$i\f$-th element
    * \f$g_i\f$ of the cyclic group \f$\{ g_1, \dots, g_p \}\f$ integers (respectively polynomials) modulo
    * \f$n\f$ (respectively \f$P(z)\f$), with \f$g_1 = 1\f$. Applying a stride with parameter \f$g_i\f$ is equivalent
    * to shifting the block elements by \f$ i \f$ positions, which leads to (square) circulant matrix-blocks if we go through 
    * the stride parameters (as column indexs) in the increasing order of  \f$ i \f$. i.e in the cyclic order of the group.
    * 
    *
    * 
    *
    * A \f$n \times n\f$ circulant matrix \f$\boldsymbol C\f$ with elements
    * \f$C_{i,j}\f$ is completely defined in terms of its first column
    * \f$\boldsymbol c = (c_1,\dots,c_s)\f$, as \f$C_{i,j} = c_{(i - j) \bmod n
    * + 1}\f$.
    *
    * Reference: http://en.wikipedia.org/wiki/Circulant_matrix
    */
   class Stride {
   public:
      typedef StorageTraits::size_type size_type;
      typedef StorageTraits::value_type value_type;

      Stride(Storage<LR, EmbeddingType::MULTILEVEL, COMPRESS, PLO> storage, value_type stride):
         m_storage(std::move(storage)),
         m_stride(stride),
         m_row(findRow(stride))
      {}

      size_type operator() (size_type i) const
      {
#if 0
         i = Compress::compressIndex(i, m_storage.sizeParam().numPoints());
         // find group start
         Level level = 0;
         size_type start = 0; 
         while (i >= start + m_storage.indices(level).size())
            start += m_storage.indices(level++).size();

         return start + ((i - start + m_row) % m_storage.indices(level).size());
#endif

         i = Compress::compressIndex(i, m_storage.sizeParam().numPoints());

         // find group start
         Level level = 0;
         size_type start = 0; 
         size_type stop = 0;
         size_type size = 0;
         while (true) {
            size = m_storage.indices(level).size();
            stop = start + size;
            if (i < stop)
               break;
            start = stop;
            level++;
         }

         size_type seqsize = m_storage.indices().size();
         

         if (PLO == PerLevelOrder::CYCLIC){
             if (LR == LatticeType::ORDINARY){
               if (not Compress::symmetric() and m_storage.sizeParam().base() == (value_type)(2) and size >= 2) {
                  // in base 2, on each level, we have 2 circulant half-blocks instead of
                  // a single circulant block
                  // see Cools et al. (2006)
                  bool reverse = m_row >= seqsize / 2;
                  size /= 2;
                  if ((i >= start + size) xor reverse)
                     start += size;
               }
             }

             // apply the circulant property; the level is repeated vertically (we apply a shift of m_row positions)
             const size_type k = (seqsize - m_row + (i - start)) % size;
             return start + k;
         }

         else if(PLO == PerLevelOrder::BASIC){
            auto& subgroup = m_storage.indices(level);
            // We apply the stride
            value_type x = (level == 0) ? value_type(1) : Compress::compressIndex((subgroup[i-start] * m_stride) %  subgroup.modulus() , subgroup.modulus());
            
            // compute the index of x in the list of emelents coprime with subgroup.modulus() in the same order as in GeneratingValues
            // The index is computed manually sinsce its a simple case:  subgroup.modulus() is a power of a prime number or an irreductible polynomial
            return start + LatticeTraits<LR>::ToIndex(x % m_storage.sizeParam().base()) +
               (LatticeTraits<LR>::NumPoints(m_storage.sizeParam().base())-1) * LatticeTraits<LR>::ToIndex(x / m_storage.sizeParam().base()) - 1 ;
         }
      }

      size_type size() const
      { return m_storage.size(); }

   private:
      Storage<LR, EmbeddingType::MULTILEVEL, COMPRESS, PLO> m_storage;
      value_type m_stride;
      size_type m_row;

      /**
       * Returns the row on which the generator value \c stride can be found.
       */
      size_type findRow(value_type stride) const
      {
#if 0
         return std::find(
               m_storage.indices().begin(),
               m_storage.indices().end(),
               stride)
            - m_storage.indices().begin();
#else
         if(PLO == PerLevelOrder::CYCLIC)
            return std::find(
               m_storage.generators().begin(),
               m_storage.generators().end(),
               Compress::compressIndex(stride, m_storage.sizeParam().modulus()))
            - m_storage.generators().begin();
        else if (PLO == PerLevelOrder::BASIC) // no nedd to compute the row of the stride 
            return 0;
#endif
      }
   };

};




/**
 * Vector permutation for embedded lattices.
 *
 * The vector embedded at level \f$m\f$ in base \f$b\f$ consists of the first
 * \f$b^m\f$ (\f$2^{m\deg(b)}\f$ for polynomial lattices) components of the internal representation of the vector (the lower
 * the level, the deeper the embedding).
 *
 * The indices are ordered by level.  Thus, refining a logical vector by adding
 * a new level op components can be achieved by appending elements at the end of
 * the physical vector.
 * Within each block, a permutation is given by the template parameter PerLevelOrder is applied:
 * - PerLevelOrder::CYCLIC corresponds to inverse cyclic subgroup corresponding to that block.
 * - PerLevelOrder::BASIC corresponds to the "natural order".
 * more details in Storage.
 *
 * \tparam COMPRESS     Compression type (either None or Symmetric).
 *                      If Compress::SYMMETRIC, the permuted indices are
 *                      compressed by assuming that the vector components at
 *                      unpermuted indices \f$i\f$ and \f$b^M - i\f$ have the
 *                      same value, for all \f$i = 0, \dots, b^M - 1\f$, where
 *                      \f$M\f$ is the maximum level of embedding.
 *
 * See the output of \ref Storage.cc for an illustration of this.
 */
template <LatticeType LR, Compress COMPRESS, PerLevelOrder PLO>
class Storage<LR, EmbeddingType::MULTILEVEL, COMPRESS, PLO> :
   public BasicStorage<Storage<LR, EmbeddingType::MULTILEVEL, COMPRESS, PLO>> {

   typedef Storage<LR, EmbeddingType::MULTILEVEL, COMPRESS, PLO> self_type;

private:
   typedef typename StorageTraits<self_type>::GroupType    GroupType;
   typedef typename StorageTraits<self_type>::GenGroupType GenGroupType;

public:
   
   typedef typename self_type::size_type  size_type;
   typedef typename self_type::Compress   Compress;
   typedef typename self_type::MeritValue MeritValue;
   typedef typename self_type::SizeParam  SizeParam;

   static std::string shortname()
   {
    if(PLO == PerLevelOrder::BASIC) return "multilevel storage, PerLevelOrder : basic" ;
    return "multilevel storage, PerLevelOrder : inverse-cyclic" ; 
    }

   Storage(const Storage& other):
      BasicStorage<Storage>(other.sizeParam()),
      m_groups(other.m_groups),
      m_genGroup(other.m_genGroup)
   {
      // This constructor should not be written explicitly.
      // This is a workaround for a bug in LLVM.
   }

   Storage(SizeParam sizeParam):
      BasicStorage<Storage>(std::move(sizeParam)),
      m_groups(this->sizeParam().maxLevel() + 1)
      
   {
      if(COMPRESS == LatBuilder::Compress::SYMMETRIC && (LR == LatticeType::POLYNOMIAL || LR == LatticeType::DIGITAL))
        throw std::invalid_argument("Storage(): No symmetric kernel implemented for polynomial");

    
      PerLevelOrderTraits<PLO, LR, COMPRESS>::initializeGroups(this->sizeParam().base(), this->sizeParam().maxLevel(),m_groups, m_genGroup);
      
   }

   size_type virtualSize() const
   { return this->sizeParam().numPoints(); }

   MeritValue createMeritValue(Real value) const
   { return RealVector(this->sizeParam().maxLevel() + 1, value); }

   const GroupType& indices() const
   { return m_groups.back(); }

   const GroupType& indices(Level i) const
   { return m_groups[i]; }

   const GenGroupType& generators() const
   { return m_genGroup; }

   /**
    * Sequence of ranges of indices corresponding to embedded levels.
    */
   class LevelRanges {
   public:

      typedef boost::numeric::ublas::range value_type;
      typedef size_t size_type;

      /**
       * Constructor.
       *
       * \param size       Size parameter, used to specify the number of points
       *                   \f$n\f$.
       */
      LevelRanges(SizeParam size):
         m_sizeParam(std::move(size)) {}

      /**
       * Default constructor.
       */
      LevelRanges() {}

      /**
       * Returns the lattice size parameter.
       */
      const SizeParam& sizeParam() const
      { return m_sizeParam; }

      /**
       * Returns the number of levels.
       */
      const size_t size() const
      { return sizeParam().maxLevel() + 1; }

   private:
      SizeParam m_sizeParam;

   public:

      /**
       * Constant iterator.
       */
      class const_iterator : public boost::iterators::iterator_facade<const_iterator,
         const value_type,                      // value
         boost::iterators::forward_traversal_tag,          // traversal
         const value_type,                      // reference
         ptrdiff_t>                             // difference
      {
      public:
         struct end_tag {};

         explicit const_iterator(const LevelRanges& seq):
            m_seq(&seq), m_level(0), m_start(0), m_stop(1)
         {}

         const_iterator(const LevelRanges& seq, end_tag):
            m_seq(&seq),
            m_level(
                  m_seq->sizeParam().base() == 0 ?
                  0 :
                  m_seq->sizeParam().maxLevel() + 1
                  ),
            m_start{0},
            m_stop{0}
         {}

         const_iterator():
            m_seq(nullptr), m_level(0), m_start(0), m_stop(0)
         {}

         Level level() const
         { return m_level; }

      private:
         friend class boost::iterators::iterator_core_access;

         void increment()
         {
            m_start = m_stop;
            m_stop *= LatticeTraits<LR>::NumPoints(m_seq->sizeParam().base());
            m_level++;
         }

         bool equal(const const_iterator& other) const
         { return m_seq == other.m_seq and m_level == other.m_level; }

         const value_type dereference() const
         { return value_type(Compress::size(m_start), Compress::size(m_stop)); }

         ptrdiff_t distance_to(const const_iterator& other) const
         { return m_seq == other.m_seq ? other.m_level - m_level : std::numeric_limits<ptrdiff_t>::max(); }

      private:
         const LevelRanges* m_seq;
         Level m_level;
         size_type m_start;
         size_type m_stop;
      };

      /**
       * Returns an iterator pointing to the first element in the seq.
       */
      const_iterator begin() const
      { return const_iterator(*this); }

      /**
       * Returns an iterator pointing past the last element in the seq.
       */
      const_iterator end() const
      { return const_iterator(*this, typename const_iterator::end_tag{}); }
   };

   /**
    * Creates and returns a new instance of LevelRanges.
    */
   LevelRanges levelRanges() const
   { return LevelRanges(this->sizeParam()); }

private:
   std::vector<GroupType> m_groups;
   GenGroupType m_genGroup;
};

}

#endif
