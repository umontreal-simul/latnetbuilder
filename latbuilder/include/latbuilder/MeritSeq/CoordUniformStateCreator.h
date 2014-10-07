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

#ifndef LATBUILDER__MERIT_SEQ__COORD_SYM_STATE_CREATOR_H
#define LATBUILDER__MERIT_SEQ__COORD_SYM_STATE_CREATOR_H

#include "latbuilder/MeritSeq/CoordUniformState.h"
#include "latbuilder/MeritSeq/ConcreteCoordUniformState.h"
#include "latbuilder/WeightsDispatcher.h"
#include "latbuilder/CombinedWeights.h"
#include "latbuilder/Storage.h"
#include "latbuilder/ClonePtr.h"

namespace LatBuilder { namespace MeritSeq {

template <LatType LAT, Compress COMPRESS>
using CoordUniformStateList = std::list<ClonePtr<MeritSeq::CoordUniformState<LAT, COMPRESS>>>;

class CoordUniformStateCreator {
public:
   /**
    * Returns a new vector of empty states for the evaluation algorithm.
    */
   template <LatType L, Compress C, class WEIGHTS>
   static
   CoordUniformStateList<L, C>
   create(const Storage<L, C>& storage, const WEIGHTS& weights)
   {
      CoordUniformStateList<L, C> list;
      WeightsDispatcher::dispatch<AddCoordUniformStates>(weights, storage, list);
      return list;
   }

private:

   template <typename WEIGHTS>
   struct AddCoordUniformStates {
      template <LatType L, Compress C>
      void operator()(
            const WEIGHTS& weights,
            const Storage<L, C>& storage,
            CoordUniformStateList<L, C>& list
            ) const
      { addCoordUniformStates(weights, storage, list); }
   };

   template <LatType L, Compress C>
   static void addCoordUniformStates(
         const CombinedWeights& weights,
         const Storage<L, C>& storage,
         CoordUniformStateList<L, C>& list
         )
   {
      for (const auto& w : weights.list())
         WeightsDispatcher::dispatch<AddCoordUniformStates>(*w, storage, list);
   }

   template <class WEIGHTS, LatType L, Compress C>
   static void addCoordUniformStates(
         const WEIGHTS& weights,
         const Storage<L, C>& storage,
         CoordUniformStateList<L, C>& list
         )
   {
      list.push_back(ClonePtr<MeritSeq::CoordUniformState<L, C>>(
               new MeritSeq::ConcreteCoordUniformState<L, C, WEIGHTS>(storage, weights)
               ));
   }

};


}}

#endif
