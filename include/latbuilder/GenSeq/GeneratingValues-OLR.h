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

#ifndef LATBUILDER__GENSEQ__GENERATING_VALUES_OLR_H
#define LATBUILDER__GENSEQ__CENERATING_VALUES_OLR_H

#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/Util.h"


#include <cstdlib>
#include <vector>
#include <limits>

namespace LatBuilder { namespace GenSeq {


  
  template<Compress COMPRESS,class TRAV>
  struct GeneratingValuesTraits<GeneratingValues<LatticeType::ORDINARY,COMPRESS,TRAV>>{
    typedef size_t size_type;
    typedef LatticeTraits<LatticeType::ORDINARY>::GenValue value_type;
    typedef LatticeTraits<LatticeType::ORDINARY>::Modulus Modulus;

    static std::string name()
      { return std::string("coprime integers / "); }

  };

/**
 * Indexed sequence of integers coprime with a specified modulus.
 *
 * This class assigns a unique index \f$i\f$ to each integer \f$k \in \mathbb
 * Z_n^* = \{1,\dots,n-1\}\f$ that is coprime with \f$n\f$, in a particular
 * order intrinsic to the Chinese remainder theorem.
 *
 * Let \f$n = n_1 \times \dots \times n_\ell\f$, where \f$n_j = b_j^{p_j}\f$ for
 * \f$j=1,\dots,\ell\f$, and where \f$b_1 < \dots < b_\ell\f$ are \f$\ell\f$
 * distinct prime numbers with respective integer powers \f$p_1,\dots,p_\ell\f$.
 * The Chinese remainder theorem states that there is an isomorphism between
 * \f$\mathbb Z_n^*\f$ and \f$Z_n = \mathbb Z_{n_1}^* \times
 * \dots \times \mathbb Z_{n_\ell}^*\f$ that maps \f$k \in \mathbb Z_n\f$ to
 * \f$\boldsymbol k = (k_1, \dots, k_\ell) = (k \bmod n_1, \dots, k \bmod
 * n_\ell) \in Z_n\f$.  Note that \f$k\f$ and \f$n\f$ are coprime if and only if
 * \f$k_j \bmod b_j \neq 0\f$ for each \f$j=1,\dots,\ell\f$.
 *
 * The sequence is sorted such that element \f$\boldsymbol k\f$ has index
 * \f[
 *   \sum_{j=1}^\ell \left(
 *       k_j - \left\lfloor \frac{k_j}{b_j} \right\rfloor - 1
 *    \right)
 *    \prod_{i=1}^{j-1} \varphi(n_i).
 * \f]
 * For example, \f$i=0\f$ corresponds to \f$\boldsymbol k = (1, 1, \dots, 1)\f$
 * and \f$i=1\f$, to \f$\boldsymbol k = (3, 1, \dots, 1)\f$ if \f$b_1 = 2\f$ and
 * \f$p_1 \geq 2\f$, or to \f$\boldsymbol k = (2, 1, \dots, 1)\f$ if \f$b_1 >
  2\f$.
 *
 * Symmetric compression (see LatBuilder::Compress::SYMMETRIC) consists in
 * applying the transformation \f$k \mapsto \min(k, n-k)\f$.  Because either the
 * sequence element \f$k\f$ or the sequence element \f$n-k\f$ is needed to
 * obtain the value \f$\min(n,n-k)\f$, it suffices to consider only the first
 * half of the sequence, i.e. the elements associated with the lower half of all
 * possible values for \f$k_\ell\f$.  This works because \f$k\f$ sits in the
 * first half if and only if \f$n-k\f$ sits in the second half: if \f$(k_1,
 * \dots, k_\ell)\f$ maps to \f$k\f$, then \f$(n_1 - k_1, \dots, n_\ell -
 * k_\ell)\f$ maps to \f$n - k\f$.  This is easy to prove by observing that
 * \f$(n - k) \bmod n_j = n_j - k_j\f$.
 *
 * Source:
 * http://en.wikipedia.org/wiki/Chinese_remainder_theorem
 *
 * \tparam COMPRESS  Type of compression.
 * \tparam TRAV      Traversal policy.
 *
 * \sa LatBuilder::Compress
 */

template <Compress COMPRESS , class TRAV >
class GeneratingValues<LatticeType::ORDINARY, COMPRESS, TRAV> :
   public Traversal::Policy<GeneratingValues<LatticeType::ORDINARY,COMPRESS, TRAV>, TRAV> {

   typedef GeneratingValues<LatticeType::ORDINARY, COMPRESS, TRAV> self_type;
   typedef Traversal::Policy<self_type, TRAV> TraversalPolicy;
   typedef CompressTraits<COMPRESS> Compress;
public:
    /**
    * Modulus type.
    */
   typedef typename GeneratingValuesTraits<self_type>::Modulus Modulus;
   /**
    * Value type.
    */
   typedef typename GeneratingValuesTraits<self_type>::value_type value_type;

   /**
    * Size type.
    */
   typedef typename GeneratingValuesTraits<self_type>::size_type size_type;

   /**
    * Traversal type.
    */
   typedef TRAV Traversal;

   static std::string name()
   { return GeneratingValuesTraits<self_type>::name()+ Compress::name() + " / " + Traversal::name(); }

   /**
    * Constructor.
    *
    * \param modulus    modulus relative to which all numbers in the sequence
    *                   are coprime.
    * \param trav       Traversal instance.
    */
   GeneratingValues(Modulus modulus = (Modulus)(1), Traversal trav = Traversal());

   /**
    * Cross-traversal copy-constructor.
    */
   template <class TRAV2>
   GeneratingValues(
         const GeneratingValues<LatticeType::ORDINARY, COMPRESS, TRAV2>& other,
         Traversal trav = Traversal()):
      TraversalPolicy(std::move(trav)),
      m_modulus(other.m_modulus),
      m_size(other.m_size),
      m_basis(other.m_basis)
   {}

   /**
    * Rebinds the traversal type.
    */
   template <class TRAV2>
   struct RebindTraversal {
      typedef GeneratingValues<LatticeType::ORDINARY,COMPRESS, TRAV2> Type;
   };

   /**
    * Returns a copy of this object, but using a different traversal policy.
    */
   template <class TRAV2>
   typename RebindTraversal<TRAV2>::Type rebind(TRAV2 trav) const
   { return typename RebindTraversal<TRAV2>::Type{*this, std::move(trav)}; }

   /**
    * Returns the modulus.
    */
   Modulus modulus() const
   { return m_modulus; }

   /**
    * Returns the size of the sequence.
    *
    * The size is the value of Euler's totient function.
    */
   size_type size() const
   { return m_size; }

   /**
    * Returns the element at index \c i.
    */
   value_type operator[](size_type i) const;

private:
   template <LatBuilder::LatticeType,LatBuilder::Compress , class > friend class GeneratingValues;

   Modulus m_modulus;
   size_type m_size;
   std::vector<detail::GeneratingValuesBasisElement<Modulus>> m_basis;
};

}}

//========================================================================
// IMPLEMENTATION
//========================================================================

namespace LatBuilder { namespace GenSeq {

template <Compress COMPRESS, class TRAV>
GeneratingValues<LatticeType::ORDINARY, COMPRESS, TRAV>::GeneratingValues(
      Modulus modulus,
      Traversal trav):
   TraversalPolicy(std::move(trav)),
   m_modulus(modulus),
   m_size(1)
{
   auto factors = primeFactorsMap(m_modulus);
   m_basis.reserve(factors.size());
   for (const auto& b : factors) {
      const auto bk = intPow(b.first, b.second);
      const auto m = m_modulus / bk;
      detail::GeneratingValuesBasisElement<Modulus> e{
         bk / b.first * (b.first - 1), // totient
         b.first - 1                   // leap
      };
      long long elem = m * egcd(bk, m).second;
      if (elem < 0)
         elem += (1 - elem / (long long)(m_modulus)) * (long long)(m_modulus);
      e.elem = (uInteger)(elem) % m_modulus;
      m_size *= e.totient;
      m_basis.push_back(std::move(e));
   }
   m_size = Compress::size(m_size + 1) - 1;
}

template <Compress COMPRESS, class TRAV>
auto GeneratingValues<LatticeType::ORDINARY, COMPRESS, TRAV>::operator[](size_type i) const -> value_type
{
   long long ret = 0;
   for (const auto& e : m_basis) {
      const ldiv_t qr = ldiv(i, e.totient);
      i = qr.quot;
      ret += (qr.rem + (qr.rem / e.leap) + 1) * e.elem;
   }
   return Compress::compressIndex(ret % modulus(), modulus());
}

}}

#endif
// vim: ft=cpp.doxygen
