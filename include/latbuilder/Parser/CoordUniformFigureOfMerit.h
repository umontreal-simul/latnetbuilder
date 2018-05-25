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
template <LatticeType LR>
struct CoordUniformFigureOfMerit {

   struct ParseKernel {
      template <class KERNEL, typename FUNC, typename... ARGS>
      void operator()(
            KERNEL kernel,
            std::unique_ptr<LatticeTester::Weights> weights,
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
         std::unique_ptr<LatticeTester::Weights> weights,
         FUNC&& func, ARGS&&... args)
   {

      try {
         const auto norm = boost::lexical_cast<Real>(strNorm);
         if (norm == 2) {
            Kernel<LR>::parse(str, ParseKernel(), std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
            return;
         }
      }
      catch (boost::bad_lexical_cast&) {}
      throw BadCoordUniformFigureOfMerit("norm must be `2' for the coordinate-uniform implementation");
   }
};

}}

#endif
