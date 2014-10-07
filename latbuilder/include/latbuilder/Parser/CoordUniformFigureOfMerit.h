// Copyright (c) 2012, 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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
#include "latbuilder/CoordUniformFigureOfMerit.h"

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid figure of merit.
 */
class BadCoordUniformFigureOfMerit : public ParserError {
public:
   BadCoordUniformFigureOfMerit(const std::string& message):
      ParserError("cannot parse coordinate-uniform figure of merit string: " + message)
   {}
};

/**
 * Parser for coordinate-uniform figures of merit.
 */
struct CoordUniformFigureOfMerit {

   struct ParseKernel {
      template <class KERNEL, typename FUNC, typename... ARGS>
      void operator()(
            KERNEL kernel,
            std::unique_ptr<LatCommon::Weights> weights,
            FUNC&& func, ARGS&&... args
            ) const
      {
         func(
               LatBuilder::CoordUniformFigureOfMerit<KERNEL>(
                  std::move(weights),
                  std::move(kernel)
                  ),
               std::forward<ARGS>(args)...
             );
      }
   };

   /**
    * Parses a string specifying a coordinate-uniform figure of merit.
    *
    * Example strings: <code>P2</code>, <code>P4</code>, <code>P6</code>
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
         if (norm == 2) {
            Kernel::parse(str, ParseKernel(), std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
            return;
         }
      }
      catch (boost::bad_lexical_cast&) {}
      throw BadCoordUniformFigureOfMerit("norm must be `2' for the coordinate-uniform implementation");
   }
};

}}

#endif
