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

#ifndef LATBUILDER__PARSER__PROJ_DEP_MERIT_H
#define LATBUILDER__PARSER__PROJ_DEP_MERIT_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/Kernel.h"
#include "latbuilder/ProjDepMerit/CoordSym.h"
#include "latbuilder/ProjDepMerit/Spectral.h"

#include "latcommon/NormaBestLat.h"


namespace LatBuilder { namespace Parser {


/**
 * Exception thrown when trying to parse an invalid combiner.
 */
class BadProjDepMerit : public ParserError {
public:
   BadProjDepMerit(const std::string& message):
      ParserError("cannot parse projection-dependent figure of merit string: " + message)
   {}
};

/**
 * Parser for projection-dependent figures of merit.
 */
struct ProjDepMerit {

   struct ParseCoordSym {
      template <class KERNEL, typename FUNC, typename... ARGS>
      void operator()(
            KERNEL kernel,
            FUNC&& func, ARGS&&... args
            ) const
      {
         func(
               LatBuilder::ProjDepMerit::CoordSym<KERNEL>(std::move(kernel)),
               std::forward<ARGS>(args)...
             );
      }
   };

   /**
    * Parses a string specifying a projection-dependent figure of merit.
    *
    * Example strings: <code>spectral</code>, <code>P2</code>, <code>P4</code>, <code>R1</code>, <code>R1.5</code>
    *
    * \throws BadProjDepMerit On failure.
    */
   template <typename FUNC, typename... ARGS>
   static void parse(const std::string& str,  FUNC&& func, ARGS&&... args)
   {
      // try coordinate-symmetric
      try {
         Kernel::parse(str, ParseCoordSym(), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
         return;
      }
      catch (BadKernel& e) {}

      // try spectral
      if (str == "spectral") {
         func(
               LatBuilder::ProjDepMerit::Spectral<LatCommon::NormaBestLat>(1.0),
               std::forward<ARGS>(args)...
             );
         return;
      }

      throw BadProjDepMerit(str);
   }
};

}}

#endif
