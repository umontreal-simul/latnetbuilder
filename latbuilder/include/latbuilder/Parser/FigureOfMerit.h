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

#ifndef LATBUILDER__PARSER__FIGURE_OF_MERIT_H
#define LATBUILDER__PARSER__FIGURE_OF_MERIT_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/WeightedFigureOfMerit.h"
#include "latbuilder/Parser/CoordSymFigureOfMerit.h"
#include "latbuilder/FigureOfMerit.h"

namespace LatBuilder { namespace Parser {

/**
 * Parser for generic weighted figures of merit.
 */
struct FigureOfMerit {
   /**
    * Parses a string specifying a weighted figure of merit and a string
    * specifying weights.
    *
    * Example strings: <code>sum:P2</code>, <code>CS:sum:P2</code>, <code>sum:spectral</code>, <code>max:spectral:2</code>
    */
   template <typename FUNC, typename... ARGS>
   static void parse(
         const std::string& str,
         std::unique_ptr<LatCommon::Weights> weights,
         FUNC&& func, ARGS&&... args)
   {
      auto strCS = splitPair<>(str, ':');
      if (strCS.first == "CS")
         CoordSymFigureOfMerit::parse(strCS.second, std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
      else
         WeightedFigureOfMerit::parse(str, std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
   }
};


}}

#endif
