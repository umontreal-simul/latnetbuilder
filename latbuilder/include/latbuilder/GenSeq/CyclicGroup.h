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

#ifndef LATBUILDER__GENSEQ__CYCLIC_GROUP_H
#define LATBUILDER__GENSEQ__CYCLIC_GROUP_H

#include "latbuilder/Traversal.h"
#include "latbuilder/CompressTraits.h"

#include "latcommon/UtilLC.h"

#include <iterator>
#include <boost/iterator/iterator_facade.hpp>


namespace LatBuilder { namespace GenSeq {

/// Group ordering type.
enum class GroupOrder : bool { DIRECT = false, INVERSE = true };
/// Returns the inverse group order.
constexpr GroupOrder operator!(GroupOrder order) { return order == GroupOrder::DIRECT ? GroupOrder::INVERSE : GroupOrder::DIRECT; }

/**
 * Default traversal policy for cyclic groups.
 */
class CyclicGroupTraversal {
public:
   typedef size_t size_type;

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

private:
   size_type m_size;
};

/**
 * Cyclic group.
 *
 * Cyclic group \f$\{ 1, g, g^2, \dots, g^{\varphi(b^m) - 1} \}\f$ of integers
 * modulo \f$b^m\f$, where \f$b\f$ is a prime base, \f$g\f$ is the generator of
 * the group, and \f$\varphi\f$ is Euler's totient function.
 * 
 * The generator of the group is computed by the class constructor.
 *
 * \tparam COMPRESS  Compression type.  If Compress::SYMMETRIC, only the first
 *                   half of the group is considered and an element value of
 *                   \f$a\f$ is mapped to \f$\min(a, b^m - a)\f$.
 *
 * \remark In base 2, the group is the union of two cyclic groups rather than a
 * single cyclic group.
 */
template <Compress COMPRESS = Compress::NONE,
         class TRAV = CyclicGroupTraversal,
         GroupOrder ORDER = GroupOrder::DIRECT>
class CyclicGroup :
   public Traversal::Policy<CyclicGroup<COMPRESS, TRAV, ORDER>, TRAV> {

   typedef CyclicGroup<COMPRESS, TRAV, ORDER> self_type;
   typedef Traversal::Policy<self_type, TRAV> TraversalPolicy;
   typedef CompressTraits<COMPRESS> Compress;

public:
   typedef size_t size_type;
   typedef Modulus value_type;

   static constexpr LatBuilder::Compress compress() { return COMPRESS; }

   /**
    * Traversal type.
    */
   typedef TRAV Traversal;

   static std::string name()
   { return std::string("cyclic group / ") + Compress::name() + " / " + Traversal::name(); }

   /**
    * Constructor for an empty group.
    */
   CyclicGroup(Traversal trav = Traversal()):
      TraversalPolicy(std::move(trav)),
      m_base(0), m_power(0), m_modulus(0), m_gen(0) {}

   /**
    * Constructor for the cyclic group of integers modulo \f$b^m\f$, where
    * \f$b\f$ is a prime base.
    * \param base       Prime base \f$b\f$.
    * \param power      Power \f$m\f$ of the base.
    * \param trav       Traversal instance.
    */
   CyclicGroup(Modulus base, Level power, Traversal trav = Traversal());

   /**
    * Cross-traversal copy-constructor.
    */
   template <class TRAV2>
   CyclicGroup(
         const CyclicGroup<COMPRESS, TRAV2, ORDER>& other,
         Traversal trav = Traversal()):
      TraversalPolicy(std::move(trav)),
      m_base(other.m_base),
      m_power(other.m_power),
      m_modulus(other.m_modulus),
      m_gen(other.m_gen)
   {}

   /**
    * Rebinds the traversal type.
    */
   template <class TRAV2>
   struct RebindTraversal {
      typedef CyclicGroup<COMPRESS, TRAV2, ORDER> Type;
   };

   /**
    * Returns a copy of this object, but using a different traversal policy.
    */
   template <class TRAV2>
   typename RebindTraversal<TRAV2>::Type rebind(TRAV2 trav) const
   { return typename RebindTraversal<TRAV2>::Type{*this, std::move(trav)}; }

   /**
    * Returns the base of the group modulus.
    */
   Modulus base() const
   { return m_base; }

   /**
    * Returns the power of the base of the group modulus.
    */
   Modulus power() const
   { return m_power; }

   /**
    * Returns the cardinality of the full group.
    * \remark If \c COMPRESS is Compress::NONE, this is the same as #size().
    */
   Modulus fullSize() const
   { return base() == 0 ? 0 : (base() - 1) * modulus() / base(); }

   /**
    * Returns the cardinality of the group part specified by \c COMPRESS.
    */
   Modulus size() const
   { return modulus() == 0 ? 0 : modulus() == 1 ? 1 : Compress::size(modulus()) - Compress::size(modulus() / base()); }

   /**
    * Returns the modulus \f$b^m\f$ of the cyclic group.
    */
   Modulus modulus() const
   { return m_modulus; }

   /**
    * Returns the generator \f$g\f$ for the group.
    */
   Modulus generator() const
   { return m_gen; }

   /**
    * Returns the element at index \c i.
    */
   value_type operator[](size_type i) const;

   /**
    * Returns the group generated by the inverse generator.
    */
   CyclicGroup<COMPRESS, TRAV, !ORDER> inverse() const
   { return CyclicGroup<COMPRESS, TRAV, !ORDER>(*this); }

   /**
    * Returns the subgroup at level \c level.
    */
   CyclicGroup subgroup(Level level) const
   { return CyclicGroup(*this, level); }

   /**
    * Returns the smallest generator for the group of cyclic integers modulo
    * \f$b^m\f$.
    * \param base    \f$b\f$
    * \param power   \f$m\f$
    * \param checkPrime    If \c true, checks if the base is actually prime.
    *
    * The algorithm is described in \cite mCOH93a .
    */
   static Modulus smallestGenerator(Modulus base, Level power, bool checkPrime = true);


private:
   template <LatBuilder::Compress, class, GroupOrder> friend class CyclicGroup;

   /**
    * Constructs a subgroup of \c group for level \c level, using the same generator.
    */
   CyclicGroup(const CyclicGroup& group, Level level);

   /**
    * Constructs the inverse group of \c group, using its generator to find the
    * inverse generator.
    */
   CyclicGroup(const CyclicGroup<COMPRESS, TRAV, !ORDER>& group);
   friend class CyclicGroup<COMPRESS, TRAV, !ORDER>;


   Modulus m_base;
   Level m_power;
   Modulus m_modulus;
   Modulus m_gen;
};

}} // namespace

//================================================================================
// Implementation
//================================================================================

#include "latbuilder/Util.h"

namespace LatBuilder { namespace GenSeq {

//================================================================================

template <Compress COMPRESS, class TRAV, GroupOrder ORDER>
Modulus CyclicGroup<COMPRESS, TRAV, ORDER>::smallestGenerator(Modulus base, Level power, bool checkPrime)
{
   if (base == 2) {
      return 5;
   }

   if (base < 2)
      throw std::invalid_argument("smallestGenerator(): base must be >= 2");

   if (checkPrime and not LatCommon::IsPrime(base))
      throw std::invalid_argument("smallestGenerator(): n must be prime");

   auto factors = primeFactors(base - 1);

   Modulus g = 1;
   while (++g <= base) {
      auto factor = factors.begin();
      while (factor != factors.end() and
            modularPow(g, (base - 1) / *factor, base) != 1)
         ++factor;
      if (factor == factors.end())
         break;
   }
   if (g > base)
      throw std::logic_error("cannot find primitive root");

   if (power == 1)
      return g;

   if (base * base < base)
      // detected overflow
      throw std::runtime_error("smallestGenerator: base too large");

   if (modularPow(g, base - 1, base * base) == 1)
      g += base;

   return g;
}

//================================================================================

template <Compress COMPRESS, class TRAV, GroupOrder ORDER>
CyclicGroup<COMPRESS, TRAV, ORDER>::CyclicGroup(const CyclicGroup<COMPRESS, TRAV, ORDER>& other, Level level):
   TraversalPolicy(static_cast<const TraversalPolicy&>(other)),
   m_base(other.m_base),
   m_power(level)
{
   if (m_power > other.m_power)
      throw std::invalid_argument("subgroup level is higher than group level");

   m_modulus = intPow(m_base, m_power);
   m_gen = other.m_gen;
}

//================================================================================

template <Compress COMPRESS, class TRAV, GroupOrder ORDER>
CyclicGroup<COMPRESS, TRAV, ORDER>::CyclicGroup(const CyclicGroup<COMPRESS, TRAV, !ORDER>& other):
   TraversalPolicy(static_cast<const TraversalPolicy&>(other)),
   m_base(other.m_base),
   m_power(other.m_power),
   m_modulus(other.m_modulus),
   m_gen(other.m_gen)
{
   if (m_base >= 2)
      m_gen = modularPow(m_gen, fullSize() - 1, modulus());
}

//================================================================================

template <Compress COMPRESS, class TRAV, GroupOrder ORDER>
CyclicGroup<COMPRESS, TRAV, ORDER>::CyclicGroup(Modulus base, Level power, Traversal trav):
   TraversalPolicy(std::move(trav)),
   m_base(base),
   m_power(power)
{
   m_modulus = intPow(m_base, m_power);
   if (m_base >= 2) {
      m_gen = smallestGenerator(m_base, m_power);
      if (ORDER == GroupOrder::INVERSE)
         m_gen = modularPow(m_gen, fullSize() - 1, modulus());
   }
   else
      m_gen = 0;
}

//================================================================================

template <Compress COMPRESS, class TRAV, GroupOrder ORDER>
auto CyclicGroup<COMPRESS, TRAV, ORDER>::operator[](size_type i) const -> value_type
{
   if (i == 0)
      return 1;

   const auto m = modulus();
   auto k = modularPow(generator(), i, m);

   // In base 2, the whole group cannot be generated at once, we must
   // mulitiply by -1 = m_modulus - 1 to generate the second half of the
   // group.
   //
   // Reference:
   // J. A. Gallian. Contemporary Base Algebra. Houghton Mifflin, 4th edition,
   // 1998. Page 104.

   if (base() == 2 and i >= size() / 2)
      k = k * (m - 1) % m;

   return Compress::compressIndex(k, m);
}

}} // namespace

namespace LatBuilder { namespace Traversal {
   template <typename SEQ>
   class Policy<SEQ, GenSeq::CyclicGroupTraversal> :
      public GenSeq::CyclicGroupTraversal {

   public:
      typedef SEQ Seq;
      typedef size_t size_type;
      typedef Modulus value_type;

      /**
       * Constructor.
       */
      Policy(GenSeq::CyclicGroupTraversal trav):
         GenSeq::CyclicGroupTraversal(std::move(trav))
      {}

      /**
       * Immutable iterator type.
       */
      class const_iterator : public boost::iterator_facade<const_iterator,
         const value_type,             // value
         boost::forward_traversal_tag, // traversal
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
         friend class boost::iterator_core_access;

         value_type dereference() const   
         { return m_value == 1 ? 1 : CompressTraits<Seq::compress()>::compressIndex(m_value, m_seq->modulus()); }

         void increment()
         {
            m_index++;
            m_value = (m_value * m_seq->generator()) % m_seq->modulus();

            // Generate the second half of the base-2 group (see note in operator[])
            if (m_value == 1 and m_index < m_seq->size())
               m_value = m_seq->modulus() - 1;
         }

         bool equal(const const_iterator& other) const
         { return m_seq == other.m_seq and m_index == other.m_index; }

         ptrdiff_t distance_to(const const_iterator& other) const
         { return m_seq == other.m_seq ? other.m_index - m_index : std::numeric_limits<ptrdiff_t>::max(); }

      private:
         const Seq* m_seq;
         size_type m_index;
         Modulus m_value;
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
      { return const_iterator(seq(), std::min(size(), (size_type)seq().size())); }

   private:
      const Seq& seq() const
      { return static_cast<const Seq&>(*this); }
   };
}}

#endif
