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

#ifndef LATBUILDER__PARSER__PROJ_DEP_MERIT_H
#define LATBUILDER__PARSER__PROJ_DEP_MERIT_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/Kernel.h"
#include "latbuilder/ProjDepMerit/CoordUniform.h"
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
template< Lattice LR>
struct ProjDepMerit {

   struct ParseCoordUniform {
      template <class KERNEL, typename FUNC, typename... ARGS>
      void operator()(
            KERNEL kernel,
            FUNC&& func, ARGS&&... args
            ) const
      {
         func(
               LatBuilder::ProjDepMerit::CoordUniform<KERNEL>(std::move(kernel)),
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
   static void parse(const std::string& str,  FUNC&& func, ARGS&&... args);
   
};




template<>
template <typename FUNC, typename... ARGS>
   void ProjDepMerit<Lattice::INTEGRATION>::parse(const std::string& str,  FUNC&& func, ARGS&&... args)
   {
      // try coordinate-uniform
      try {
         Kernel<Lattice::INTEGRATION>::parse(str, ParseCoordUniform(), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
         return;
      }
      catch (BadKernel& e) {}

      // try spectral
        if (str == "spectral") {
           func(
                 LatBuilder::ProjDepMerit::Spectral<LatCommon::NormaBestLat>(2.0),
                 std::forward<ARGS>(args)...
               );
           return;
        }
      

      throw BadProjDepMerit(str);
   }

template<>
template <typename FUNC, typename... ARGS>
   void ProjDepMerit<Lattice::POLYNOMIAL>::parse(const std::string& str,  FUNC&& func, ARGS&&... args)
   {
      // try coordinate-uniform
      try {
         Kernel<Lattice::POLYNOMIAL>::parse(str, ParseCoordUniform(), std::forward<FUNC>(func), std::forward<ARGS>(args)...);
         return;
      }
      catch (BadKernel& e) {}

      
      

      throw BadProjDepMerit(str);
   }

/*
extern template struct ProjDepMerit<Lattice::INTEGRATION>;
extern template struct ProjDepMerit<Lattice::POLYNOMIAL>;
*/

}}

#endif
