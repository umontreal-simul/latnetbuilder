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

#ifndef LATBUILDER__PARSER__KERNEL_H
#define LATBUILDER__PARSER__KERNEL_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Kernel/RAlpha.h"
#include "latbuilder/Kernel/PAlpha.h"

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid kernel.
 */
class BadKernel : public ParserError {
public:
   BadKernel(const std::string& message):
      ParserError("cannot parse coordinate-symmetric kernel string: " + message)
   {}
};

/**
 * Parser for kernels for coordinate-symmetric figures of merit.
 */
struct Kernel {
   /**
    * Parses a string specifying a kernel for the coordinate-symmetric figure of
    * merit, like the \f$\mathcal P_\alpha\f$ and the \f$\mathcal R_\alpha\f$
    * figures of merit.
    *
    * Example strings: <code>P2</code>, <code>P4</code>, <code>P6</code>, <code>R1</code>, <code>R1.5</code>, <code>R2</code>
    *
    * \return A shared pointer to a newly created object or \c nullptr on failure.
    */
   template <typename FUNC, typename... ARGS>
   static void parse(const std::string& str,  FUNC&& func, ARGS&&... args)
   {
      try {
         if (str[0] == 'P') {
            auto alpha = boost::lexical_cast<unsigned int>(str.substr(1));
            func(LatBuilder::Kernel::PAlpha(alpha), std::forward<ARGS>(args)...);
            return;
         }
         else if (str[0] == 'R') {
            auto alpha = boost::lexical_cast<Real>(str.substr(1));
            func(LatBuilder::Kernel::RAlpha(alpha), std::forward<ARGS>(args)...);
            return;
         }
      }
      catch (boost::bad_lexical_cast&) {}
      throw BadKernel(str);
   }
};

}}

#endif
