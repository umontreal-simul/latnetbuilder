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

#ifndef LATBUILDER__GENSEQ__GENERATING_VALUES_PLR_H
#define LATBUILDER__GENSEQ__GENERATING_VALUES_PLR_H

#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/Util.h"
#include <NTL/GF2XFactoring.h>


#include <cstdlib>
#include <vector>
#include <limits>

namespace LatBuilder { namespace GenSeq {


  
   template<Compress COMPRESS, class TRAV>
  struct GeneratingValuesTraits<GeneratingValues<LatticeType::POLYNOMIAL,COMPRESS,TRAV>>{
    typedef size_t size_type;
    typedef LatticeTraits<LatticeType::POLYNOMIAL>::GenValue value_type;
    typedef LatticeTraits<LatticeType::POLYNOMIAL>::Modulus Modulus;
    
    static std::string name()
      { return std::string("coprime polynomials / ") ; }
  };

/**
 * Indexed sequence of Polynomials coprime with a specified polynomial modulus.
 *
 * we note a polynomial by specifing its variable (for example \f$T(z)\f$), and whenever \f$T(z)=\sum_{i=0}^{w} t_{i}z^i \in \mathbb{Z}_{2}[z]\f$, $T$ stands for the integer \f$T = \sum_{i=0}^{w} t_{i}2^i \in \mathbb{N}\f$. 
 * 
 * Let \f$P(z) = \prod_{j=1}^{l} p_{j}(z)^{\alpha_{j}}\f$  such that \f$p_{j}(z)\f$ is irreductible over \f$\mathbb{Z}_{2}\f$, and let \f$\varphi(P(z)) = \left | (\mathbb{Z}_{2}[z]/(P(z)))^{*} \right |\f$. 
 * The chinese remainder theorem states that there is an isomorphism between \f$(\mathbb{Z}_{2}[z]/(P(z)))^{*}\f$ and \f$(\mathbb{Z}_{2}[z]/(p_{1}^{\alpha_{1}}(z)))^{*}\times...\times (\mathbb{Z}_{2}[z]/(p_{l}^{\alpha_{l}}(z)))^{*}\f$ 
 * 
 * \f{eqnarray*}{
 * \Psi :\quad &(\mathbb{Z}_{2}[z]/(P(z)))^{*} &&\longrightarrow \quad && (\mathbb{Z}_{2}[z]/(p_{1}^{\alpha_{1}}(z)))^{*}\times...\times (\mathbb{Z}_{2}[z]/(p_{l}^{\alpha_{l}}(z)))^{*}\\
 * & k(z) &&\longmapsto &&\big(k_{1}(z),...,k_{l}(z)\big) = \big(k(z) \mod  p_{1}^{\alpha_{1}}(z),...,k(z) \mod  p_{l}^{\alpha_{l}}(z)\big) \\
 * \f}
 * We order elements of \f$(\mathbb{Z}_{2}[z]/(P(z)))^{*}\f$ as follows: we identify \f$k(z) \in (\mathbb{Z}_{2}[z]/(P(z)))^{*}\f$ with \f$\Psi(k(z)) = \big(k_{1}(z),...,k_{l}(z)\big)  \in (\mathbb{Z}_{2}[z]/(p_{1}^{\alpha_{1}}(z)))^{*}\times...\times (\mathbb{Z}_{2}[z]/(p_{l}^{\alpha_{l}}(z)))^{*}\f$, for which we choose the dictionnary order, with \f$(\mathbb{Z}_{2}[z]/(p_{j}^{\alpha_{j}}(z)))^{*}\f$ ordered as follows:
 * \f[
 * h_{1}(z) <_{p_{j}}  h_{2}(z) \quad \text{iff } \left\{
 * \begin{array}{ll}
 * Q_1 < Q_2 \\
 * \text{or } Q_1 = Q_2 \text{ and } R_1 < R_2
 * \end{array}
 * \right.
 * \f]
 * with 
 * \f[
 * \left\{
 * \begin{array}{ll}
 * h_{1}(z) = p_{j}(z) Q_{1}(z) + R_{1}(z) \\
 * h_{2}(z) = p_{2}(z) Q_{2}(z) + R_{2}(z)
 * \end{array}
 * \right., \quad \deg(R_{1}(z)),\deg(R_{2}(z)) < \deg(p_{j}(z))
 * \f]
 * Equivalently, to each \f$k(z) \in (\mathbb{Z}_{2}[z]/(P(z)))^{*}\f$ we assign the index
 * \f[
 *   i_{k(z)} = \sum_{j=1}^{l} (R_{j} + (2^{\deg(p_j(z))}-1)Q_{j}-1) \prod_{g=1}^{j-1} \varphi(p_{g}^{\alpha_{g}}(z)),
 * \f]
 * with
 * \f[
 *   k_{j}(z) = p_{j}(z) Q_{j}(z) + R_{j}(z), \quad \deg(R_{j}(z)) < \deg(p_{j}(z))
 * \f] 
 *
 * If \f$i = \sum_{j=1}^{l} r_{j} \prod_{g=1}^{j-1}\varphi(p_{g}^{\alpha_{g}}(z)) \in \{0,\dots, \varphi(P(z))-1\}\f$ is an index, \f$(k_1(z),\dots,k_l(z))\f$ can be computed using the formula 
 * \f[
 *   k_{j}(z) = p_j(z) Q_j(z) + R_j(z)
 * \f] 
 * with 
 *   \f[
 *   \left\{
 *   \begin{array}{ll}
 *   Q_{j} = \left \lfloor \frac{r_{j}}{2^{\deg(p_{j})}-1} \right \rfloor\\
 *   R_{j} = r_{j} \mod (2^{\deg(p_{j})}-1) +1
 *   \end{array}
 *   \right.
 *   \f]
 */

template <Compress COMPRESS , class TRAV >
class GeneratingValues<LatticeType::POLYNOMIAL, COMPRESS, TRAV> :
   public Traversal::Policy<GeneratingValues<LatticeType::POLYNOMIAL,COMPRESS, TRAV>, TRAV> {

   typedef GeneratingValues<LatticeType::POLYNOMIAL, COMPRESS, TRAV> self_type;
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
         const GeneratingValues<LatticeType::POLYNOMIAL, COMPRESS, TRAV2>& other,
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
      typedef GeneratingValues<LatticeType::POLYNOMIAL,COMPRESS, TRAV2> Type;
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
   template <LatBuilder::LatticeType, LatBuilder::Compress , class > friend class GeneratingValues;

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
GeneratingValues<LatticeType::POLYNOMIAL, COMPRESS, TRAV>::GeneratingValues(
      Modulus modulus,
      Traversal trav):
   TraversalPolicy(std::move(trav)),
   m_modulus(modulus),
   m_size(1)
{
   NTL::vector< NTL::Pair< Polynomial, long > > factors ;
   CanZass(factors, m_modulus); // calls "Cantor/Zassenhaus" algorithm from <NTL/GF2XFactoring.h>
   m_basis.resize(factors.size());

   size_t index = 0;
   for (const auto& b : factors) {
      const auto bk = intPow(b.a, b.b); // b.a is the first element, b.b the second
      const auto m = m_modulus / bk;
      size_t totient = intPow(2, b.b * deg(b.a)) / intPow(2,deg(b.a)) * (intPow(2,deg(b.a)) - 1);
      size_t leap = intPow(2,deg(b.a)) -1;
      detail::GeneratingValuesBasisElement<Modulus> e{
         totient,  // totient
         leap,      // leap
         b.a        // primeBase (irreductible polynomial)
      };
      Polynomial gcd,s,t;
      XGCD(gcd,s,t,bk,m); // bk*s + m*t = gcd = 1
      Polynomial elem = m * t;
      e.elem = elem % m_modulus;
      m_size *= e.totient;
      m_basis[index] = std::move(e);
      index ++;
   }
   m_size = Compress::size(m_size + 1) - 1;
}

template <Compress COMPRESS, class TRAV>
auto GeneratingValues<LatticeType::POLYNOMIAL, COMPRESS, TRAV>::operator[](size_type i) const -> value_type
{
   value_type ret = value_type(0);
   for (const auto& e : m_basis) {
      const ldiv_t qr = ldiv(i, e.totient);
      i = qr.quot;
      Polynomial Q = PolynomialFromInt(qr.rem / e.leap);
      Polynomial R = PolynomialFromInt(qr.rem % e.leap + 1);
      ret +=  ((e.primeBase * Q + R) * e.elem) % modulus();
   }

   return Compress::compressIndex(ret , modulus());
   
}

}}

#endif
// vim: ft=cpp.doxygen
