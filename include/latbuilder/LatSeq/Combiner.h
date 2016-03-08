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
 * Sequence of lattices based on a combination of integer sequences.
 *
 * Contains all lattices with the same size parameter and dimension, with
 * components of the generating vectors taken from a user-specified vector of
 * integer sequences, one corresponding to each coordinate.
 *
 * \tparam LAT       Type of lattice.
 * \tparam GENSEQ    Type of sequence of generator values.
 * \tparam POLICY    See SeqCombiner.
 */
template <
   LatType LAT,
   class GENSEQ,
   template <class> class POLICY>
class Combiner :
   public BridgeSeq<
      Combiner<LAT, GENSEQ, POLICY>,
      SeqCombiner<GENSEQ, POLICY>,
      LatDef<LAT>,
      BridgeIteratorCached>
{
   typedef Combiner<LAT, GENSEQ, POLICY> self_type;

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
         SizeParam<LAT> sizeParam,
         std::vector<GenSeq> genSeqs
         ):
      self_type::BridgeSeq_(Base(std::move(genSeqs))),
      m_sizeParam(std::move(sizeParam))
   {}

   /**
    * Returns the size parameter of the lattices in the sequence.
    */
   const SizeParam<LAT>& sizeParam() const
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
   SizeParam<LAT> m_sizeParam;
};

/// Creates a lattice sequence based on a combination of sequences of integers.
template <template <class> class POLICY, LatType LAT, class GENSEQ>
Combiner<LAT, GENSEQ, POLICY>
combine(
      SizeParam<LAT> size,
      std::vector<GENSEQ> genSeqs
      ) {
   return Combiner<LAT, GENSEQ, POLICY>(std::move(size), std::move(genSeqs));
}

}}

#endif
