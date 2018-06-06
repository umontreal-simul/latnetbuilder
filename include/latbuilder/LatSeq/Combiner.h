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

#ifndef LATBUILDER__LAT_SEQ__COMBINER_H
#define LATBUILDER__LAT_SEQ__COMBINER_H

#include "latbuilder/BridgeSeq.h"
#include "latbuilder/SeqCombiner.h"
#include "latbuilder/Types.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/LatDef.h"

namespace LatBuilder {

namespace LatSeq {

/**
 * Sequence of lattices based on a combination of sequences of generator values.
 *
 * Contains all lattices with the same size parameter and dimension, with
 * components of the generating vectors taken from a user-specified vector of
 * sequences of generator values, one corresponding to each coordinate.
 *
 * \tparam PST       Type of lattice.
 * \tparam GENSEQ    Type of sequence of generator values.
 * \tparam POLICY    See SeqCombiner.
 */
template <
   LatticeType LR,
   PointSetType PST,
   class GENSEQ,
   template <class> class POLICY>
class Combiner :
   public BridgeSeq<
      Combiner<LR, PST, GENSEQ, POLICY>,
      SeqCombiner<GENSEQ, POLICY>,
      LatDef<LR, PST>,
      BridgeIteratorCached>
{
   typedef Combiner<LR, PST, GENSEQ, POLICY> self_type;

public:

   typedef typename self_type::Base Base;
   typedef typename self_type::value_type value_type;
   typedef typename self_type::size_type size_type;
   typedef GENSEQ GenSeq;

   /**
    * Constructor.
    *
    * \param sizeParam     Common lattice size parameter.
    * \param genSeqs       Sequence of sequences of generator values each component of the
    *                      generating vector can take.
    */
   Combiner(
         SizeParam<LR, PST> sizeParam,
         std::vector<GenSeq> genSeqs
         ):
      self_type::BridgeSeq_(Base(std::move(genSeqs))),
      m_sizeParam(std::move(sizeParam))
   {}

   /**
    * Returns the size parameter of the lattices in the sequence.
    */
   const SizeParam<LR, PST>& sizeParam() const
   { return m_sizeParam; }

   /**
    * Returns the dimension of the lattices.
    */
   Dimension latDimension() const
   { return this->base().seqs().size(); }

   /**
    * Computes and returns the output value.
    */
   value_type element(const typename Base::const_iterator& it) const
   { return value_type(m_sizeParam, *it); }

private:
   SizeParam<LR, PST> m_sizeParam;
};

/// Creates a lattice sequence based on a combination of sequences of generator values.
template <template <class> class POLICY,LatticeType LR, PointSetType PST, class GENSEQ>
Combiner<LR, PST, GENSEQ, POLICY>
combine(
      SizeParam<LR, PST> size,
      std::vector<GENSEQ> genSeqs
      ) {
   return Combiner<LR, PST, GENSEQ, POLICY>(std::move(size), std::move(genSeqs));
}

}}

#endif
