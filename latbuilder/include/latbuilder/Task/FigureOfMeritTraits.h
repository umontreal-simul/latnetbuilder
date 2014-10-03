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

#ifndef LATBUILDER__TASK_FIGURE_OF_MERIT_TRAITS_H
#define LATBUILDER__TASK_FIGURE_OF_MERIT_TRAITS_H

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/CoordSymFigureOfMerit.h"
#include "latbuilder/Storage.h"
#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/MeritSeq/CoordSymCBC.h"
#include "latbuilder/MeritSeq/CoordSymInnerProd.h"

namespace LatBuilder { namespace Task {

/**
 * Traits for figures of merit.
 *
 * \tparam LAT          Type of lattice.
 * \tparam COMPRESS     Type of compression.
 * \tparam FIGURE       Type of figure of merit.
 *
 * Supported types of figures of merit: any instance of the
 * WeightedFigureOfMerit or of the CoordSymFigureOfMerit templates.
 *
 * These traits define:
 * - a CBC type that is a component-by-component merit sequence (either an
 *   instance of MeritSeq::CBC or of MeritSeq::CoordSymCBC) that is appropriate
 *   for use with the FIGURE type;
 * - an init() function that takes an instance of Task::Search as its argument
 *   and that performs special actions depending on the type of CBC algorithm.
 */
template <LatType LAT, Compress COMPRESS, class FIGURE>
struct FigureOfMeritTraits;

template <LatType LAT, Compress COMPRESS, class PROJDEP, template <class> class ACC>
struct FigureOfMeritTraits<LAT, COMPRESS, WeightedFigureOfMerit<PROJDEP, ACC>> {
   typedef MeritSeq::CBC<LAT, COMPRESS, PROJDEP, ACC> CBC;
   /**
    * Utility function to connect WeightedFigureOfMerit::OnProgress with
    * Search::MinObserver.
    */
   template <class SEARCH> static void init(SEARCH& search)
   {
      typedef typename Storage<LAT, COMPRESS>::MeritValue MeritValue;
      typedef bool (SEARCH::MinObserver::*ProgressCallback)(const MeritValue&) const;
      ProgressCallback progress = &SEARCH::MinObserver::progress;

      // We want to interrupt the evaluation of the figure of merit when it
      // reaches a value larger than that threshold, so we pass the partial
      // sum/max through the low-pass filter.
      //
      // Connect the onProgress signal to the function that checks that the
      // current minimum value is greater than the partial sum/max.
      //
      // NOTE: this doesn't work for embedded lattices.
      search.cbc().evaluator().onProgress().connect(boost::bind(
               progress,
               &search.minObserver(),
               _1
               ));

      // truncate the sum over projections only if no filters are applied
      // downstream
      search.minObserver().setTruncateSum(search.filters().empty());
   }
};

template <LatType LAT, Compress COMPRESS, class KERNEL>
struct FigureOfMeritTraits<LAT, COMPRESS, CoordSymFigureOfMerit<KERNEL>> {
   typedef MeritSeq::CoordSymCBC<LAT, COMPRESS, KERNEL, MeritSeq::CoordSymInnerProd> CBC;
   template <class SEARCH> static void init(SEARCH& search) {}
};

}}

#endif
