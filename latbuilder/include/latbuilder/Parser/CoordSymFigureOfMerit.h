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

#ifndef LATBUILDER__PARSER__COORD_SYM_FIGURE_OF_MERIT_H
#define LATBUILDER__PARSER__COORD_SYM_FIGURE_OF_MERIT_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/Kernel.h"
#include "latbuilder/CoordSymFigureOfMerit.h"

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid figure of merit.
 */
class BadCoordSymFigureOfMerit : public ParserError {
public:
   BadCoordSymFigureOfMerit(const std::string& message):
      ParserError("cannot parse coordinate-symmetric figure of merit string: " + message)
   {}
};

/**
 * Parser for coordinate-symmetric figures of merit.
 */
struct CoordSymFigureOfMerit {

   struct ParseKernel {
      template <class KERNEL, typename FUNC, typename... ARGS>
      void operator()(
            KERNEL kernel,
            std::unique_ptr<LatCommon::Weights> weights,
            FUNC&& func, ARGS&&... args
            ) const
      {
         func(
               LatBuilder::CoordSymFigureOfMerit<KERNEL>(
                  std::move(weights),
                  std::move(kernel)
                  ),
               std::forward<ARGS>(args)...
             );
      }
   };

   /**
    * Parses a string specifying a coordinate-symmetric figure of merit.
    *
    * Example strings: <code>sum:P2</code>, <code>sum:P4</code>, <code>sum:P6</code>
    */
   template <typename FUNC, typename... ARGS>
   static void parse(
         const std::string& strNorm,
         const std::string& str,
         std::unique_ptr<LatCommon::Weights> weights,
         FUNC&& func, ARGS&&... args)
   {
      try {
         const auto norm = boost::lexical_cast<Real>(strNorm);
         if (norm == 2)
            Kernel::parse(str, ParseKernel(), std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
      }
      catch (boost::bad_lexical_cast&) {
         throw BadCoordSymFigureOfMerit("norm must be `2' for the coordinate-symmetric implementation");
      }
   }
};

}}

#endif
