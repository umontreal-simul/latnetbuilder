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
struct WeightedFigureOfMerit {

   template <template <typename> class ACC>
   struct ParseProjDepMerit {
      template <class PROJDEP, typename FUNC, typename... ARGS>
      void operator()(
            PROJDEP projDepMerit,
            Real normType,
            std::unique_ptr<LatCommon::Weights> weights,
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
    * Example strings: <code>sum:P2</code>, <code>sum:spectral</code>, <code>max:spectral:2</code>
    *
    * \throws BadNorm On failure.
    */
   template <typename FUNC, typename... ARGS>
   static void parse(
         const std::string& strNorm,
         const std::string& str,
         std::unique_ptr<LatCommon::Weights> weights,
          FUNC&& func, ARGS&&... args)
   {
      if (strNorm == "inf") {
         ProjDepMerit::parse(str, ParseProjDepMerit<Functor::Max>(), 1.0, std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
         return;
      }
      try {
         const auto norm = boost::lexical_cast<Real>(strNorm);
         ProjDepMerit::parse(str, ParseProjDepMerit<Functor::Sum>(), norm, std::move(weights), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
         return;
      }
      catch (boost::bad_lexical_cast&) {}
      throw BadNorm(strNorm);
   }
};


}}

#endif
