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
 * \tparam LAT       Type of lattice.
 * \tparam GENSEQ    Type of sequence of sequences of generator values.
 *
 * \sa GenSeqSeq
 */
template <LatType LAT, class GENSEQ>
class Korobov :
   public Combiner<
      LAT,
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
         const SizeParam<LAT>& sizeParam,
         const GENSEQ& genSeq,
         Dimension latDimension):
      Combiner<LAT, GenSeq::PowerSeq<GENSEQ>, Zip>(
            sizeParam,
            makeGenSeqs(sizeParam, genSeq, latDimension))
   {}

private:
   static std::vector<GenSeq::PowerSeq<GENSEQ>> makeGenSeqs(
         const SizeParam<LAT>& sizeParam,
         const GENSEQ& genSeq,
         Dimension dimension)
   {
      std::vector<GenSeq::PowerSeq<GENSEQ>> vec;
      vec.reserve(dimension);
      for (unsigned int coord = 0; coord < dimension; coord++)
         vec.push_back(GenSeq::PowerSeq<GENSEQ>{
               genSeq,
               coord,
               sizeParam.numPoints()});
      return vec;
   }
};

/// Creates a Korobov lattice sequence.
template <LatType LAT, class GENSEQ>
Korobov<LAT, GENSEQ>
korobov(
      const SizeParam<LAT>& size,
      const GENSEQ& genSeqs,
      Dimension dimension
      ) {
   return Korobov<LAT, GENSEQ>(size, genSeqs, dimension);
}

}}

#endif
