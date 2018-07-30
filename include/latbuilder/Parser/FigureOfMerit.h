// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
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

#ifndef LATBUILDER__PARSER__FIGURE_OF_MERIT_H
#define LATBUILDER__PARSER__FIGURE_OF_MERIT_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/WeightedFigureOfMerit.h"
#include "latbuilder/Parser/CoordUniformFigureOfMerit.h"
#include "latbuilder/FigureOfMerit.h"

namespace LatBuilder { namespace Parser {

/**
 * Parser for generic weighted figures of merit.
 */
  template<LatticeType LR>
struct FigureOfMerit {
   /**
    * Parses a string specifying a weighted figure of merit and a string
    * specifying weights.
    *
    * Example strings: <code>P2</code>, <code>CU:P2</code>, <code>spectral</code>, <code>spectral:2</code>
    */
   template <typename FUNC, typename... ARGS>
   static void parse(
         const std::string& strNorm,
         const std::string& str,
         unsigned int interlacingFactor,
         std::unique_ptr<LatticeTester::Weights> weights,
         FUNC&& func, ARGS&&... args)
   {
      auto strCS = splitPair<>(str, ':');
      if (strCS.first == "CU")
         CoordUniformFigureOfMerit<LR>::parse(strNorm, strCS.second, interlacingFactor, std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
      else
         WeightedFigureOfMerit<LR>::parse(strNorm, str, interlacingFactor, std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
   }
};


}}

#endif
