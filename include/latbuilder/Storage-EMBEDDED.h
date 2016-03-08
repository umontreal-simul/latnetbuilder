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

#ifndef LATBUILDER__STORAGE_EMBEDDED_H
#define LATBUILDER__STORAGE_EMBEDDED_H

#include "latbuilder/SizeParam.h"
#include "latbuilder/GenSeq/CyclicGroup.h"

#include <map>
#include <stdexcept>

namespace LatBuilder {

template <Compress COMPRESS>
struct StorageTraits<Storage<LatType::EMBEDDED, COMPRESS>> {
   typedef Modulus               size_type;
   typedef CompressTraits<COMPRESS> Compress;
   typedef RealVector            MeritValue;
   typedef LatBuilder::SizeParam<LatType::EMBEDDED> SizeParam;

   typedef GenSeq::CyclicGroup<COMPRESS, Traversal::Forward, GenSeq::GroupOrder::INVERSE> GroupType; 
   typedef GenSeq::CyclicGroup<COMPRESS, Traversal::Forward, GenSeq::GroupOrder::DIRECT> GenGroupType; 

   /**
    * Unpermuted permutation.
    */
   class Unpermute {
   public:
      typedef StorageTraits::size_type size_type;
      typedef std::pair<boost::numeric::ublas::range, size_type> AddressType;

      Unpermute(Storage<LatType::EMBEDDED, COMPRESS> storage):
         m_storage(std::move(storage))
      {
         // initialize addresses
         m_indexToAddress[0] = std::make_pair(boost::numeric::ublas::range{0, 1}, 0);
         size_t start = 1;
         typename GroupType::value_type mult = m_storage.sizeParam().numPoints();
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
         const auto where = indexToAddress(i);
         return where.first.start() + where.second;
      }

      size_type size() const
      { return m_storage.virtualSize(); }

   private:
      typedef std::map<typename GroupType::value_type, AddressType> MapType;

      Storage<LatType::EMBEDDED, COMPRESS> m_storage;
      MapType m_indexToAddress;

      /**
       * Returns the address of the element at index \c i.
       *
       * The address is a pair of which the first element is a range on the
       * elements indices for the level, and the sub-index on that level.
       */
      const AddressType& indexToAddress(size_t i) const
      {
         i = Compress::compressIndex(i, m_storage.sizeParam().numPoints());
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

         const Modulus base = m_storage.sizeParam().base();

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
    * The elements on each level are visited by jumping across a certain number
    * of elements (the stride length) periodically.
    *
    * Consider the unpermuted vector \f$\boldsymbol v = (v_1, \dots, v_n)\f$ for
    * some positive integer \f$n\f$.  The \f$j\f$-th component of the vector
    * with stride length \f$a\f$ has value \f$v_{j a \bmod n}\f$.
    *
    * The stride length \f$a\f$ can be identified to the \f$i\f$-th element
    * \f$g_i\f$ of the cyclic group \f$\{ g_1, \dots, g_p \}\f$ integers modulo
    * \f$n\f$, with \f$g_1 = 1\f$.
    *
    * In the multilevel case, we can define a series of per-level vectors that
    * regroup the elements of vector \f$\boldsymbol v\f$ corresponding to
    * distinct levels.
    *
    * Each per-level vector can be seen a distinct row of an horizontal matrix
    * composed of (square) circulant blocks.
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

      Stride(Storage<LatType::EMBEDDED, COMPRESS> storage, size_type stride):
         m_storage(std::move(storage)),
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

         if (not Compress::symmetric() and m_storage.sizeParam().base() == 2 and size >= 2) {
            // in base 2, on each level, we have 2 circulant half-blocks instead of
            // a single circulant block
            // see Cools et al. (2006)
            bool reverse = m_row >= seqsize / 2;
            size /= 2;
            if ((i >= start + size) xor reverse)
               start += size;
         }

         // apply the circulant property; the level is repeated vertically
         const size_type k = (seqsize - m_row + (i - start)) % size;
         return start + k;
      }

      size_type size() const
      { return m_storage.size(); }

   private:
      Storage<LatType::EMBEDDED, COMPRESS> m_storage;
      size_type m_row;

      /**
       * Returns the row on which the generator value \c stride can be found.
       */
      size_type findRow(size_type stride) const
      {
#if 0
         return std::find(
               m_storage.indices().begin(),
               m_storage.indices().end(),
               stride)
            - m_storage.indices().begin();
#else
         return std::find(
               m_storage.generators().begin(),
               m_storage.generators().end(),
               Compress::compressIndex(stride, m_storage.sizeParam().numPoints()))
            - m_storage.generators().begin();
#endif
      }
   };

};

/**
 * Vector permutation for embedded lattices.
 *
 * The vector embedded at level \f$m\f$ in base \f$b\f$ consists of the first
 * \f$b^m\f$ components of the internal representation of the vector (the lower
 * the level, the deeper the embedding).
 *
 * The indices are ordered by level.  Thus, refining a logical vector by adding
 * a new level op components can be achieved by appending elements at the end of
 * the physical vector.
 * Within each level, a permutation given by the inverse cyclic subgroup
 * corresponding to that level is applied.
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
template <Compress COMPRESS>
class Storage<LatType::EMBEDDED, COMPRESS> :
   public BasicStorage<Storage<LatType::EMBEDDED, COMPRESS>> {

   typedef Storage<LatType::EMBEDDED, COMPRESS> self_type;

private:
   typedef typename StorageTraits<self_type>::GroupType    GroupType;
   typedef typename StorageTraits<self_type>::GenGroupType GenGroupType;

public:

   typedef typename self_type::size_type  size_type;
   typedef typename self_type::Compress   Compress;
   typedef typename self_type::MeritValue MeritValue;
   typedef typename self_type::SizeParam  SizeParam;

   static std::string shortname()
   { return "multilevel storage"; }

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
      m_groups(this->sizeParam().maxLevel() + 1),
      m_genGroup(GenGroupType(this->sizeParam().base(), this->sizeParam().maxLevel()))
   {
      // initialize groups
      GroupType maxLevelGroup(this->sizeParam().base(), this->sizeParam().maxLevel());
      for (Level level = 0; level <= this->sizeParam().maxLevel(); level++)
         m_groups[level] = maxLevelGroup.subgroup(level);
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
            m_stop *= m_seq->sizeParam().base();
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
