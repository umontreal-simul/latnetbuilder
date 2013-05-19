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

#ifndef LATBUILDER__GEN_SEQ__CREATOR_H
#define LATBUILDER__GEN_SEQ__CREATOR_H

#include "latbuilder/GenSeq/CyclicGroup.h"
#include "latbuilder/GenSeq/PowerSeq.h"
#include "latbuilder/SizeParam.h"

namespace LatBuilder { namespace GenSeq {

/**
 * Creator for sequences of generator values.
 *
 * It is an adaptor for the different constructor signatures of the generator
 * sequences.
 * It allows to use the same constructor arguments regardless of the sequence
 * type.
 *
 */
template <typename SEQ>
struct Creator {
   typedef SEQ result_type;

   /**
    * Creates a new sequence object.
    */
   template <LatType L, typename... ARGS>
   static result_type create(
         const SizeParam<L>& sizeParam,
         ARGS&&... t
         )
   { return result_type(sizeParam.numPoints(), std::forward<ARGS>(t)...); }
};

/**
 * Creator specialization for cyclic groups.
 */
template <Compress COMPRESS, class TRAV, GroupOrder ORDER>
struct Creator<CyclicGroup<COMPRESS, TRAV, ORDER>> {
   typedef CyclicGroup<COMPRESS, TRAV, ORDER> result_type;

   template <typename... ARGS>
   static result_type create(
         const SizeParam<LatType::EMBEDDED>& sizeParam,
         ARGS&&... t
         )
   { return result_type(sizeParam.base(), sizeParam.maxLevel(), std::forward<ARGS>(t)...); }

   template <typename... ARGS>
   static result_type create(
         const SizeParam<LatType::ORDINARY>& sizeParam,
         ARGS&&... t
         )
   {
      SizeParam<LatType::EMBEDDED> ml(sizeParam.numPoints());
      return result_type(ml.base(), ml.maxLevel(), std::forward<ARGS>(t)...);
   }
};

/**
 * Creator specialization for PowerSeq.
 */
template <typename SEQ>
struct Creator<PowerSeq<SEQ>> {
   typedef PowerSeq<SEQ> result_type;
   /**
    * Creates a new sequence object.
    */
   template <LatType L, typename... ARGS>
   static result_type create(
         const SizeParam<L>& sizeParam,
         unsigned int power,
         ARGS&&... t
         )
   { return result_type(Creator<SEQ>::create(sizeParam, std::forward<ARGS>(t)...), power, sizeParam.numPoints()); }
};

}}

#endif
