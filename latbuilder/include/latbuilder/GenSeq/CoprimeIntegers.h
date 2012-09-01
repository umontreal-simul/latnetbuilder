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

#ifndef LATBUILDER__GENSEQ__COPRIME_INTEGERS_H
#define LATBUILDER__GENSEQ__COPRIME_INTEGERS_H

#include "latbuilder/Util.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/CompressTraits.h"

#include <cstdlib>
#include <vector>
#include <limits>

namespace LatBuilder { namespace GenSeq {

namespace detail {
   struct CoprimeIntegersBasisElement {
      Modulus totient;
      Modulus leap;
      Modulus elem; 
      CoprimeIntegersBasisElement(Modulus t = 0, Modulus l = 0, Modulus e = 0):
         totient(t), leap(l), elem(e) {}
   };
}

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
template <Compress COMPRESS = Compress::NONE,
         class TRAV = Traversal::Forward>
class CoprimeIntegers :
   public Traversal::Policy<CoprimeIntegers<COMPRESS, TRAV>, TRAV> {

   typedef CoprimeIntegers<COMPRESS, TRAV> self_type;
   typedef Traversal::Policy<self_type, TRAV> TraversalPolicy;
   typedef CompressTraits<COMPRESS> Compress;

public:

   /**
    * Value type.
    */
   typedef Modulus value_type;

   /**
    * Size type.
    */
   typedef size_t size_type;

   /**
    * Traversal type.
    */
   typedef TRAV Traversal;

   static std::string name()
   { return std::string("coprime integers / ") + Compress::name() + " / " + Traversal::name(); }

   /**
    * Constructor.
    *
    * \param modulus    Modulus relative to which all numbers in the sequence
    *                   are coprime.
    * \param trav       Traversal instance.
    */
   CoprimeIntegers(value_type modulus = 1, Traversal trav = Traversal());

   /**
    * Cross-traversal copy-constructor.
    */
   template <class TRAV2>
   CoprimeIntegers(
         const CoprimeIntegers<COMPRESS, TRAV2>& other,
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
      typedef CoprimeIntegers<COMPRESS, TRAV2> Type;
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
   value_type modulus() const
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
   template <LatBuilder::Compress, class> friend class CoprimeIntegers;

   value_type m_modulus;
   size_type m_size;
   std::vector<detail::CoprimeIntegersBasisElement> m_basis;
};

}}

//========================================================================
// IMPLEMENTATION
//========================================================================

namespace LatBuilder { namespace GenSeq {

template <Compress COMPRESS, class TRAV>
CoprimeIntegers<COMPRESS, TRAV>::CoprimeIntegers(
      value_type modulus,
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
      detail::CoprimeIntegersBasisElement e{
         bk / b.first * (b.first - 1), // totient
         b.first - 1                   // leap
      };
      long long elem = m * egcd(bk, m).second;
      if (elem < 0)
         elem += (1 - elem / (long long)(m_modulus)) * (long long)(m_modulus);
      e.elem = (Modulus)(elem) % m_modulus;
      m_size *= e.totient;
      m_basis.push_back(std::move(e));
   }
   m_size = Compress::size(m_size + 1) - 1;
}

template <Compress COMPRESS, class TRAV>
auto CoprimeIntegers<COMPRESS, TRAV>::operator[](size_type i) const -> value_type
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
