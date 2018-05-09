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

#ifndef LATBUILDER__GENSEQ__CYCLIC_GROUP_OLR_H
#define LATBUILDER__GENSEQ__CYCLIC_GROUP_OLR_H

#include "latbuilder/GenSeq/CyclicGroup.h"



namespace LatBuilder { namespace GenSeq {

   template<>
  struct CyclicGroupTraversalTraits<CyclicGroupTraversal<LatticeType::ORDINARY>>{
    typedef size_t size_type;
    typedef LatticeTraits<LatticeType::ORDINARY>::GenValue value_type;
    typedef LatticeTraits<LatticeType::ORDINARY>::Modulus Modulus;
     static void increment(value_type& currentValue, size_type& index, const value_type& generator, const Modulus& modulus, const size_type& size){
      index ++;
      currentValue = (currentValue * generator) % modulus;
      // Generate the second half of the base-2 group 
      if (currentValue == 1 and index < size)
                 currentValue = modulus - 1;
      }

  };

  template<Compress COMPRESS,class TRAV, GroupOrder ORDER>
  struct CyclicGroupTraits<CyclicGroup<LatticeType::ORDINARY,COMPRESS,TRAV,ORDER>>{
    typedef size_t size_type;
    typedef LatticeTraits<LatticeType::ORDINARY>::GenValue value_type;
    typedef LatticeTraits<LatticeType::ORDINARY>::Modulus Modulus;
  };

template <Compress COMPRESS ,
         class TRAV ,
         GroupOrder ORDER>
class CyclicGroup < LatticeType::ORDINARY, COMPRESS, TRAV, ORDER> :
   public Traversal::Policy<CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, ORDER>, TRAV> {

   typedef CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, ORDER> self_type;
   typedef Traversal::Policy<self_type, TRAV> TraversalPolicy;
   typedef CompressTraits<COMPRESS> Compress;

public:
   typedef typename CyclicGroupTraits<self_type>::size_type size_type;
   typedef typename CyclicGroupTraits<self_type>::value_type value_type;
   typedef typename CyclicGroupTraits<self_type>::Modulus Modulus;

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
         const CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV2, ORDER>& other,
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
      typedef CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV2, ORDER> Type;
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
   Level power() const
   { return m_power; }

   /**
    * Returns the cardinality of the full group.
    * \remark If \c COMPRESS is Compress::NONE, this is the same as #size().
    */
   size_type fullSize() const
   { return base() == 0 ? 0 : (base() - 1) * modulus() / base(); }

   /**
    * Returns the cardinality of the group part specified by \c COMPRESS.
    */
   size_type size() const
    { return modulus() == 0 ? 0 : modulus() == 1 ? 1 : Compress::size(modulus()) - Compress::size(modulus() / base()); }

   /**
    * Returns the modulus \f$b^m\f$ of the cyclic group.
    */
   Modulus modulus() const
   { return m_modulus; }

   /**
    * Returns the generator \f$g\f$ for the group.
    */
   value_type generator() const
   { return m_gen; }

   /**
    * Returns the element at index \c i.
    */
   value_type operator[](size_type i) const;

   /**
    * Returns the group generated by the inverse generator.
    */
   CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, !ORDER> inverse() const
   { return CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, !ORDER>(*this); }

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
   static value_type smallestGenerator(Modulus base, Level power, bool checkPrime = true);


private:
   template <LatticeType, LatBuilder::Compress, class, GroupOrder> friend class CyclicGroup;

   /**
    * Constructs a subgroup of \c group for level \c level, using the same generator.
    */
   CyclicGroup(const CyclicGroup& group, Level level);

   /**
    * Constructs the inverse group of \c group, using its generator to find the
    * inverse generator.
    */
   CyclicGroup(const CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, !ORDER>& group);
   friend class CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, !ORDER>;


   Modulus m_base;
   Level m_power;
   Modulus m_modulus;
   value_type m_gen;
};

}} // namespace

//================================================================================
// Implementation
//================================================================================

#include "latbuilder/Util.h"
#include "latcommon/IntFactor.h"

namespace LatBuilder { namespace GenSeq {

//================================================================================

template < Compress COMPRESS, class TRAV, GroupOrder ORDER>
typename CyclicGroup<LatticeType::ORDINARY,COMPRESS, TRAV, ORDER>::value_type 
CyclicGroup<LatticeType::ORDINARY,COMPRESS, TRAV, ORDER>::smallestGenerator(Modulus base, Level power, bool checkPrime) 
{
   if (base == 2) {
      return 5;
   }

   if (base < 2)
      throw std::invalid_argument("smallestGenerator(): base must be >= 2");

   if (checkPrime and LatCommon::IntFactor::isPrime(base, 0) == LatCommon::COMPOSITE)
      throw std::invalid_argument("smallestGenerator(): n must be prime");

   auto factors = primeFactors(base - 1);

   value_type g = 1;
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

template < Compress COMPRESS, class TRAV, GroupOrder ORDER>
CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, ORDER>::CyclicGroup(const CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, ORDER>& other, Level level):
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
CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, ORDER>::CyclicGroup(const CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, !ORDER>& other):
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
CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, ORDER>::CyclicGroup(Modulus base, Level power, Traversal trav):
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
auto CyclicGroup<LatticeType::ORDINARY, COMPRESS, TRAV, ORDER>::operator[](size_type i) const -> value_type
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

#endif