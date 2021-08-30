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

#ifndef LATBUILDER__LAT_SEQ__KOROBOV_H
#define LATBUILDER__LAT_SEQ__KOROBOV_H

#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/PowerSeq.h"
#include "latbuilder/Types.h"

#include <vector>

namespace LatBuilder { namespace LatSeq {

/**
 * Sequence of Korobov lattices.
 *
 * \tparam ET       Type of lattice.
 * \tparam GENSEQ    Type of sequence of sequences of generator values.
 *
 * \sa GenSeqSeq
 */
template <LatticeType LR, EmbeddingType ET, class GENSEQ>
class Korobov :
   public Combiner<
      LR,
      ET,
      GenSeq::PowerSeq<GENSEQ>,
      Zip> {

public:
   /**
    * Constructor.
    *
    * \param sizeParam     Lattice size parameter.
    * \param genSeq        Sequence of sequences of generator values.
    * \param latDimension  Dimension of the lattices in the sequence.
    */
   Korobov(
         const SizeParam<LR, ET>& sizeParam,
         const GENSEQ& genSeq,
         Dimension latDimension):
      Combiner<LR, ET, GenSeq::PowerSeq<GENSEQ>, Zip>(
            sizeParam,
            makeGenSeqs(sizeParam, genSeq, latDimension))
   {}

private:
   static std::vector<GenSeq::PowerSeq<GENSEQ>> makeGenSeqs(
         const SizeParam<LR, ET>& sizeParam,
         const GENSEQ& genSeq,
         Dimension dimension)
   {
      std::vector<GenSeq::PowerSeq<GENSEQ>> vec;
      vec.reserve(dimension);
      for (unsigned int coord = 0; coord < dimension; coord++)
         vec.push_back(GenSeq::PowerSeq<GENSEQ>{
               genSeq,
               coord,
               sizeParam.modulus()});
      return vec;
   }
};

/// Creates a Korobov lattice sequence.
template <LatticeType LR, EmbeddingType ET, class GENSEQ>
Korobov<LR, ET, GENSEQ>
korobov(
      const SizeParam<LR, ET>& size,
      const GENSEQ& genSeqs,
      Dimension dimension
      ) {
   return Korobov<LR, ET, GENSEQ>(size, genSeqs, dimension);
}

}}

#endif
