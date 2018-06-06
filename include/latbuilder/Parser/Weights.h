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

#ifndef LATBUILDER__PARSER__WEIGHTS_H
#define LATBUILDER__PARSER__WEIGHTS_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Types.h"

#include "latticetester/ProjectionDependentWeights.h"

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
    * \param arg            Input string to be parsed as weights.
    * \param powerScale     Power to which each number parsed from input will
    *                       be raised before being assigned to a weight.
    *
    * Example strings: <code>projection-dependent:1,2:1.0</code>,
    *    <code>projection-dependent:1,2:1.0:1,2,3:0.3</code>
    *
    * \return A pointer to a newly created object or \c nullptr on failure.
    */
   static std::unique_ptr<LatticeTester::Weights>
   parseProjectionDependent(const std::string& arg, Real powerScale);

   /**
    * Parses a string specifying order-dependent weights.
    *
    * \param arg            Input string to be parsed as weights.
    * \param powerScale     Power to which each number parsed from input will
    *                       be raised before being assigned to a weight.
    *
    * Example strings: <code>order-dependent:0.1</code>, <code>order-dependent:0.1:1.0,0.7,0.3</code>
    *
    * \return A pointer to a newly created object or \c nullptr on failure.
    */
   static std::unique_ptr<LatticeTester::Weights>
   parseOrderDependent(const std::string& arg, Real powerScale);

   /**
    * Parses a string specifying product weights.
    *
    * \param arg            Input string to be parsed as weights.
    * \param powerScale     Power to which each number parsed from input will
    *                       be raised before being assigned to a weight.
    *
    * Example strings: <code>product:0.1</code>, <code>product:0.1:1.0,0.7,0.3</code>
    *
    * \return A pointer to a newly created object or \c nullptr on failure.
    */
   static std::unique_ptr<LatticeTester::Weights>
   parseProduct(const std::string& arg, Real powerScale);

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
   static std::unique_ptr<LatticeTester::Weights>
   parsePOD(const std::string& arg, Real powerScale);

   /**
    * Parses a string specifying weights.
    *
    * \param arg            Input string to be parsed as weights.
    * \param powerScale     Power to which each number parsed from input will
    *                       be raised before being assigned to a weight.
    *                       This is useful to parse weights for a figure of
    *                       merit defined as a \f$q\f$-norm with respect to the
    *                       different projections, e.g., in the
    *                       WeightedFigureOfMerit::WeightedFigureOfMerit
    *                       constructor, the weights are assumed to be already
    *                       raised to the power \f$q\f$.
    *
    * For example strings, see #parseProjectionDependent(),
    * #parseOrderDependent() and #parseProduct().
    *
    * \return A pointer to a newly created object or \c nullptr on failure.
    */
   static std::unique_ptr<LatticeTester::Weights>
   parse(const std::string& arg, Real powerScale=1.0);
};

}}

#endif
