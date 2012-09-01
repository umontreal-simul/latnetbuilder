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

#include "latbuilder/MeritSeq/CoordSymState.h"
#include "latbuilder/MeritSeq/ConcreteCoordSymState.h"
#include "latbuilder/WeightsDispatcher.h"
#include "latbuilder/CombinedWeights.h"
#include "latbuilder/Storage.h"
#include "latbuilder/ClonePtr.h"

namespace LatBuilder { namespace MeritSeq {

template <LatType LAT, Compress COMPRESS>
using CoordSymStateList = std::list<ClonePtr<MeritSeq::CoordSymState<LAT, COMPRESS>>>;

class CoordSymStateCreator {
public:
   /**
    * Returns a new vector of empty states for the evaluation algorithm.
    */
   template <LatType L, Compress C, class WEIGHTS>
   static
   CoordSymStateList<L, C>
   create(const Storage<L, C>& storage, const WEIGHTS& weights)
   {
      CoordSymStateList<L, C> list;
      WeightsDispatcher::dispatch<AddCoordSymStates>(weights, storage, list);
      return list;
   }

private:

   template <typename WEIGHTS>
   struct AddCoordSymStates {
      template <LatType L, Compress C>
      void operator()(
            const WEIGHTS& weights,
            const Storage<L, C>& storage,
            CoordSymStateList<L, C>& list
            ) const
      { addCoordSymStates(weights, storage, list); }
   };

   template <LatType L, Compress C>
   static void addCoordSymStates(
         const CombinedWeights& weights,
         const Storage<L, C>& storage,
         CoordSymStateList<L, C>& list
         )
   {
      for (const auto& w : weights.list())
         WeightsDispatcher::dispatch<AddCoordSymStates>(*w, storage, list);
   }

   template <class WEIGHTS, LatType L, Compress C>
   static void addCoordSymStates(
         const WEIGHTS& weights,
         const Storage<L, C>& storage,
         CoordSymStateList<L, C>& list
         )
   {
      list.push_back(ClonePtr<MeritSeq::CoordSymState<L, C>>(
               new MeritSeq::ConcreteCoordSymState<L, C, WEIGHTS>(storage, weights)
               ));
   }

};


}}

#endif
