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

#ifndef LATBUILDER__PARSER__WEIGHTED_FIGURE_OF_MERIT_H
#define LATBUILDER__PARSER__WEIGHTED_FIGURE_OF_MERIT_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/ProjDepMerit.h"
#include "latbuilder/WeightedFigureOfMerit.h"

#include <boost/lexical_cast.hpp>

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid accumulator.
 */
class BadNorm : public ParserError {
public:
   BadNorm(const std::string& message):
      ParserError("cannot parse norm string (should be a number or `inf'): " + message)
   {}
};


/**
 * Parser for generic weighted figures of merit.
 */
template< LatticeType LR>
struct WeightedFigureOfMerit {

   template <template <typename> class ACC>
   struct ParseProjDepMerit {
      template <class PROJDEP, typename FUNC, typename... ARGS>
      void operator()(
            PROJDEP projDepMerit,
            Real normType,
            std::unique_ptr<LatticeTester::Weights> weights,
             FUNC&& func, ARGS&&... args
            ) const
      {
         func(
               LatBuilder::WeightedFigureOfMerit<PROJDEP, ACC>(
                  normType,
                  std::move(weights),
                  std::move(projDepMerit)
                  ),
               std::forward<ARGS>(args)...
             );
      }
   };

   /**
    * Parses a string specifying a weighted figure of merit.
    *
    * Example strings: <code>P2</code>, <code>spectral</code>, <code>spectral:2</code>
    *
    * \throws BadNorm On failure.
    */
   template <typename FUNC, typename... ARGS>
   static void parse(
         const std::string& strNorm,
         const std::string& str,
         std::unique_ptr<LatticeTester::Weights> weights,
          FUNC&& func, ARGS&&... args)
   {
      if (strNorm == "inf") {
         ProjDepMerit<LR>::parse(str, ParseProjDepMerit<Functor::Max>(), 1.0, std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
         return;
      }
      try {
         const auto norm = boost::lexical_cast<Real>(strNorm);
         ProjDepMerit<LR>::parse(str, ParseProjDepMerit<Functor::Sum>(), norm, std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
         return;
      }
      catch (boost::bad_lexical_cast&) {}
      throw BadNorm(strNorm);
   }
};


}}

#endif
