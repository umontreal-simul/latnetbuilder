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

#ifndef LATBUILDER__PARSER__WEIGHTS_H
#define LATBUILDER__PARSER__WEIGHTS_H

#include "latbuilder/Parser/Common.h"

#include "latcommon/ProjectionDependentWeights.h"

#include <memory>

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid weights specificaiton.
 */
class BadWeights : public ParserError {
public:
   BadWeights(const std::string& message):
      ParserError("cannot parse weights string: " + message)
   {}
};

/**
 * Parser for weights.
 */
struct Weights {
   /**
    * Parses a string specifying projection-dependent weights.
    *
    * Example strings: <code>projection-dependent:1,2:1.0</code>,
    *    <code>projection-dependent:1,2:1.0:1,2,3:0.3</code>
    *
    * \return A pointer to a newly created object or \c nullptr on failure.
    */
   static std::unique_ptr<LatCommon::Weights>
   parseProjectionDependent(const std::string& arg);

   /**
    * Parses a string specifying order-dependent weights.
    *
    * Example strings: <code>order-dependent:0.1</code>, <code>order-dependent:0.1:1.0,0.7,0.3</code>
    *
    * \return A pointer to a newly created object or \c nullptr on failure.
    */
   static std::unique_ptr<LatCommon::Weights>
   parseOrderDependent(const std::string& arg);

   /**
    * Parses a string specifying product weights.
    *
    * Example strings: <code>product:0.1</code>, <code>product:0.1:1.0,0.7,0.3</code>
    *
    * \return A pointer to a newly created object or \c nullptr on failure.
    */
   static std::unique_ptr<LatCommon::Weights>
   parseProduct(const std::string& arg);

   /**
    * Parses a string specifying product and order-dependent (POD) weights.
    *
    * The string must have the following form:
    * <code>POD:&lt;default-Gamma&gt;:&lt;Gamma_1&gt;,&lt;Gamma_2&gt;,...:&lt;default-gamma&gt;:&lt;gamma_1&gt;,...</code>
    *
    * Example strings: <code>POD:1.0::0.1</code>, <code>POD:1.0:1.0,0.7,0.3:0.1:0.9,0.8,0.7</code>
    *
    * \return A pointer to a newly created object or \c nullptr on failure.
    */
   static std::unique_ptr<LatCommon::Weights>
   parsePOD(const std::string& arg);

   /**
    * Parses a string specifying weights.
    *
    * For example strings, see #parseProjectionDependent(),
    * #parseOrderDependent() and #parseProduct().
    *
    * \return A pointer to a newly created object or \c nullptr on failure.
    */
   static std::unique_ptr<LatCommon::Weights>
   parse(const std::string& arg);
};

}}

#endif
