// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#ifndef LATBUILDER__STORAGE_H
#define LATBUILDER__STORAGE_H

#include "latbuilder/Types.h"
#include "latbuilder/IndexMap.h"

#include <boost/numeric/ublas/vector_proxy.hpp>

#include <string>

namespace LatBuilder {



/**
 *  default per level value depending on the lattice (ordinary/polynomial) and the lattice type (ordinary/embedded).
 *
 */
template <LatticeType LR, EmbeddingType ET>
struct defaultPerLevelOrder;

template <>
struct defaultPerLevelOrder<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>{
   static const PerLevelOrder Order = PerLevelOrder::BASIC;
};
template <>
struct defaultPerLevelOrder<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>{
   static const PerLevelOrder Order = PerLevelOrder::BASIC;
};

template <>
struct defaultPerLevelOrder<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>{
   static const PerLevelOrder Order = PerLevelOrder::CYCLIC;
};

template <>
struct defaultPerLevelOrder<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>{
   static const PerLevelOrder Order = PerLevelOrder::BASIC;
};

template <>
struct defaultPerLevelOrder<LatticeType::DIGITAL, EmbeddingType::UNILEVEL>{
   static const PerLevelOrder Order = PerLevelOrder::BASIC;
};

template <>
struct defaultPerLevelOrder<LatticeType::DIGITAL, EmbeddingType::MULTILEVEL>{
   static const PerLevelOrder Order = PerLevelOrder::BASIC;
};


/**
 * Storage policy.
 *
 * A storage policy indicates how kernel values are stored and represented in a
 * vector.
 * This template class acts as a selector for specialized templates, based on
 * the type of lattice and on the type of compression.
 *
 * 
 *
 * - For (simple) ordinary lattice rules with modulud \f$n\f$:
 * The kernel values are stored in the natural order: the ith value \f$w_i\f$ is \f$w(i/n)\f$.
 * A Stride with parameter \f$a \in \mathbb{N}\f$ is the mapping that maps an index \f$i\f$ to \f$a \times i \mod n\f$.
 *  
 * 
 *
 * - For a (simple) polynomnial lattice rule with modulus \f$P(z)\f$:
 * Polynomials modulo \f$P(z)\f$ are identified with integers via the formula  \f$h=\sum a_j2^j\f$ where  \f$h(z) = \sum a_jz^j\f$. 
 * The kernel values are stored in the following order: the \f$i^{\text{th}}\f$ value \f$w_i\f$ is \f$w(\nu_m(\frac{i(z)}{P(z)}))\f$. with \f$i(z) = \sum a_jz^j\f$ where \f$i=\sum a_j2^j\f$. 
 * A Stride with parameter \f$q(z) \in \mathbb{Z}_2[z]\f$ is the mapping that maps a polynomial \f$i(z)\f$ modulo \f$P(z)\f$ with the polynomial \f$h(z) = i(z)q(z) \mod P(z)\f$ .
 *
 *   
 *
 * 
 *
 * - For embedded ordinary lattice rules with modulus \f$p^m\f$ for a prime \f$p\f$: \n
 * The kernel values are stored in \f$m+1\f$ blocks: the \f$k^{\text{th}}\f$ block (\f$k=0..m\f$) is the set \f$p^{m-k}U_{p^k}\f$. 
 * the \f$k^{\text{th}}\f$ level is the concatenation of the first \f$k\f$ blocks. 
 * The elements within each block can be ordered according to the inversed cyclic order of \f$U_{p^k}\f$. i.e the block (ordered) elements are
 * \f$\{p^{m-k}g^{-l}, l=0..\varphi(p^k)-1 \}\f$ with \f$g\f$ a generator of  \f$U_{p^k}\f$.
 * This order permits to do fast CBC exploration.  This order can be obtained by setting the PerLevelOrder template parameter to PerLevelOrder::CYCLIC.
 * One can set PerLevelOrder to PerLevelOrder::BASIC, in this case, elements within each block are ordered in the non-decreasing order.
 * PerLevelOrder::CYCLIC is set as the default order, the basic order was introduced especially for polynomial lattices, where the cyclic structure does not apply if \f$m>1\f$.
 *
 * 
 * 
 *
 * - For embedded polynomial lattice rules with modulus \f$b(z)^m\f$ for an irreductible polynomial  \f$b(z)\f$:
 * The kernel values are stored in \f$m+1\f$ blocks: the \f$k^{\text{th}}\f$ block (\f$k=0..m\f$) is the set \f$b(z)^{m-k}(\mathbb{Z}_2[z]/(b(z)^k))^*\f$.
 * The \f$k^{\text{th}}\f$ level is the concatenation of the first \f$k\f$ blocks.
 * If \f$m=1\f$, we have two block with sizes 1 and \f$2^{\deg(b(z))}-1\f$ and the elements within the second block can be ordered according to the inversed cyclic order of \f$(\mathbb{Z}_2[z]/(b(z)))^*\f$.
 * i.e the block (ordered) elements are \f$\{g^{-l}, l=0..\varphi(b(z))-1 \}\f$ with \f$g\f$ a generator of \f$(\mathbb{Z}_2[z]/(b(z)))^*\f$.
 * This order permits to do fast CBC exploration, and can be obtained by setting the PerLevelOrder template parameter to PerLevelOrder::CYCLIC.
 * If \f$m>1\f$, the cyclic structure does not apply and we have to set PerLevelOrder to PerLevelOrder::BASIC.
 * In this case, elements of \f$(\mathbb{Z}_2[z]/(b(z)^k))^*\f$ are ordered according to the order given by the class GeneratingValues.
 */
template <LatticeType LR, EmbeddingType ET, Compress COM, PerLevelOrder PLO = defaultPerLevelOrder<LR, ET>::Order > class Storage;

/**
 * Storage traits.
 *
 * Specializations of this class template must define the types:
 * - size_type: size type for vectors;
 * - Compress: a specialization of CompressTraits;
 * - MeritValue: data type for merit values;
 * - SizeParam: specilization of LatBuilder::SizeParam;
 * - Unpermute: operator that maps a logical vector index to a storage index;
 * - Stride: operator that maps a point index to a storage index (a logical
 *   vector index is first obtained by multiplying the point index (respectively the polynomial corresponding to the index)
 *  with the <em>stride</em> factor, an integer (respectively polynomial) value which
 *   normally corresponds to that of a generating vector component.
 */
template <class DERIVED>
struct StorageTraits;

/**
 * Base class for storage types.
 *
 * \tparam DERIVED   Concrete storage type.
 */
template <class DERIVED>
class BasicStorage {
protected:
   BasicStorage(const BasicStorage&) = default;

protected:
   typedef typename StorageTraits<DERIVED>::Compress  Compress;

public:

   typedef typename StorageTraits<DERIVED>::size_type size_type;
   typedef typename StorageTraits<DERIVED>::value_type value_type;
   typedef typename StorageTraits<DERIVED>::MeritValue MeritValue;
   typedef typename StorageTraits<DERIVED>::SizeParam SizeParam;
   typedef typename StorageTraits<DERIVED>::Unpermute Unpermute;
   typedef typename StorageTraits<DERIVED>::Stride    Stride;

   BasicStorage(SizeParam sizeParam):
      m_sizeParam(std::move(sizeParam))
   {}

   static std::string name()
   { return DERIVED::shortname() + " / " + Compress::name(); }

   size_type virtualSize() const
   { return derived().virtualSize(); }

   size_type size() const
   { return Compress::size(virtualSize()); }

   const SizeParam& sizeParam() const
   { return m_sizeParam; }

   bool symmetric() const
   { return Compress::symmetric(); }

   bool operator==(const BasicStorage& other) const
   { return sizeParam() == other.sizeParam(); }

   bool operator!=(const BasicStorage& other) const
   { return !operator==(other); }

   MeritValue createMeritValue(Real value) const
   { return derived().createMeritValue(value); }

   /**
    * Returns a vector proxy to access the vector's elements in their natural
    * order.
    */
   template <class V>
   boost::numeric::ublas::vector_indirect<V, IndexMap<Unpermute>> unpermuted(
         boost::numeric::ublas::vector_container<V>& vec
         ) const
   {
      return boost::numeric::ublas::vector_indirect<V, IndexMap<Unpermute>>(
            vec(),
            IndexMap<Unpermute>(Unpermute(derived()))
            );
   }

   /**
    * Returns a vector proxy to access the vector's elements with a periodic
    * jump of \c stride across the elements.
    */
   template <class V>
   boost::numeric::ublas::vector_indirect<const V, IndexMap<Stride>> strided(
         const boost::numeric::ublas::vector_container<V>& vec,
         value_type stride
         ) const
   {
      return boost::numeric::ublas::vector_indirect<const V, IndexMap<Stride>>(
            vec(),
            IndexMap<Stride>(Stride(derived(), stride))
            );
   }

private:
   SizeParam m_sizeParam;

   DERIVED& derived()
   { return static_cast<DERIVED&>(*this); }

   const DERIVED& derived() const
   { return static_cast<const DERIVED&>(*this); }
};
}

#include "latbuilder/Storage-SIMPLE.h"
#include "latbuilder/Storage-EMBEDDED.h"
#include "latbuilder/Storage-SIMPLE-DIGITAL.h"
#include "latbuilder/Storage-EMBEDDED-DIGITAL.h"

#endif
