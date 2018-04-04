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

#ifndef LATBUILDER__MERIT_SEQ__COORD_SYM_STATE_CREATOR_H
#define LATBUILDER__MERIT_SEQ__COORD_SYM_STATE_CREATOR_H

#include "latbuilder/MeritSeq/CoordUniformState.h"
#include "latbuilder/MeritSeq/ConcreteCoordUniformState.h"
#include "latbuilder/WeightsDispatcher.h"
#include "latbuilder/CombinedWeights.h"
#include "latbuilder/Storage.h"
#include "latbuilder/ClonePtr.h"

namespace LatBuilder { namespace MeritSeq {

template <LatticeType LR, LatEmbed LAT, Compress COMPRESS, PerLevelOrder PLO >
using CoordUniformStateList = std::list<ClonePtr<MeritSeq::CoordUniformState<LR, LAT, COMPRESS, PLO>>>;

class CoordUniformStateCreator {
public:
   /**
    * Returns a new vector of empty states for the evaluation algorithm.
    */
   template <LatticeType LA, LatEmbed L, Compress C, PerLevelOrder P, class WEIGHTS >
   static
   CoordUniformStateList<LA, L, C, P>
   create(const Storage<LA, L, C, P>& storage, const WEIGHTS& weights)
   {
      CoordUniformStateList<LA, L, C, P> list;
      WeightsDispatcher::dispatch<AddCoordUniformStates>(weights, storage, list);
      return list;
   }

private:

   template <typename WEIGHTS>
   struct AddCoordUniformStates {
      template <LatticeType LA, LatEmbed L, Compress C, PerLevelOrder P >
      void operator()(
            const WEIGHTS& weights,
            const Storage<LA, L, C, P>& storage,
            CoordUniformStateList<LA, L, C, P>& list
            ) const
      { addCoordUniformStates(weights, storage, list); }
   };

   template <LatticeType LA, LatEmbed L, Compress C, PerLevelOrder P >
   static void addCoordUniformStates(
         const CombinedWeights& weights,
         const Storage<LA, L, C, P>& storage,
         CoordUniformStateList<LA, L, C, P>& list
         )
   {
      for (const auto& w : weights.list())
         WeightsDispatcher::dispatch<AddCoordUniformStates>(*w, storage, list);
   }

   template <class WEIGHTS,LatticeType LA, LatEmbed L, Compress C, PerLevelOrder P >
   static void addCoordUniformStates(
         const WEIGHTS& weights,
         const Storage<LA, L, C, P>& storage,
         CoordUniformStateList<LA, L, C, P>& list
         )
   {
      list.push_back(ClonePtr<MeritSeq::CoordUniformState<LA, L, C, P>>(
               new MeritSeq::ConcreteCoordUniformState<LA, L, C, P, WEIGHTS>(storage, weights)
               ));
   }

};


}}

#endif
