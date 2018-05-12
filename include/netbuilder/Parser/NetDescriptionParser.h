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

#ifndef NETBUILDER__PARSER__NET_DESCRIPTION_PARSER_H
#define NETBUILDER__PARSER__NET_DESCRIPTION_PARSER_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/NetConstructionTraits.h"

#include <algorithm>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

namespace NetBuilder { namespace Parser {
/**
 * Exception thrown when trying to parse an invalid size parameter.
 */
class BadNetDescription : public lbp::ParserError {
public:
   BadNetDescription(const std::string& message):
      lbp::ParserError("cannot parse net description string: " + message)
   {}
};

/**
 * Parser for construction parameters.
 */
template<NetConstruction NC, PointSetType PST>
struct NetDescriptionParser {};

template<PointSetType PST>
struct NetDescriptionParser<NetConstruction::SOBOL, PST>
{
    typedef typename NetConstructionTraits<NetConstruction::SOBOL>::GenValue GenValue;
   typedef std::vector<GenValue> result_type;

   static result_type parse(CommandLine<NetConstruction::SOBOL, PST>& commandLine, const std::string& str)
   {
       std::vector<std::string> netDescriptionStrings;
       boost::split(netDescriptionStrings, str, boost::is_any_of("/"));
       unsigned int dim = 1;
       result_type genValues;
       genValues.reserve(commandLine.m_dimension);
       for(const auto& dirNumsString : netDescriptionStrings)
       {
           std::vector<std::string> dirNumbersStrings;
           boost::split(dirNumbersStrings, dirNumsString, boost::is_any_of(","));
           std::vector<uInteger> dirNumbersValues;
           dirNumbersValues.reserve(dirNumbersStrings.size());
           for (auto numStr : dirNumbersStrings)
           {
               dirNumbersValues.push_back(boost::lexical_cast<uInteger>(numStr));
           }
           GenValue genVal = GenValue(dim,std::move(dirNumbersValues));
           if(!NetConstructionTraits<NetConstruction::SOBOL>::checkGenValue(genVal, commandLine.m_designParameter))
           {
                throw BadNetDescription("Bad Sobol' direction numbers.");
           }
           genValues.push_back(std::move(genVal));
           ++dim;
       }
        if (genValues.size() != commandLine.m_dimension)
        {
           throw BadNetDescription("Missing Sobol' direction numbers.");
        }
        return genValues;
   }
};

template<PointSetType PST>
struct NetDescriptionParser<NetConstruction::POLYNOMIAL, PST>
{
    typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::GenValue GenValue;
    typedef std::vector<GenValue> result_type;

   static result_type parse(CommandLine<NetConstruction::POLYNOMIAL, PST>& commandLine, const std::string& str)
   {
       std::vector<std::string> netDescriptionStrings;
       boost::split(netDescriptionStrings, str, boost::is_any_of("/"));
       result_type genValues;
       genValues.reserve(commandLine.m_dimension);
       for(const auto& polyString : netDescriptionStrings)
       {
           GenValue genVal = polynomialParserHelper(polyString);
           if(!NetConstructionTraits<NetConstruction::POLYNOMIAL>::checkGenValue(genVal, commandLine.m_designParameter))
           {
               throw BadNetDescription("Bad generating polynomial.");
           }
           genValues.push_back(std::move(genVal));
       }
        if (genValues.size() != commandLine.m_dimension )
        {
           throw BadNetDescription("Missing generating vector.");
        }
        return genValues;
   }
};

template<PointSetType PST>
struct NetDescriptionParser<NetConstruction::EXPLICIT, PST>
{
    typedef typename NetConstructionTraits<NetConstruction::EXPLICIT>::GenValue GenValue;
    typedef std::vector<GenValue> result_type;

   static result_type parse(CommandLine<NetConstruction::EXPLICIT, PST>& commandLine, const std::string& str)
   {
       std::vector<std::string> netDescriptionStrings;
       boost::split(netDescriptionStrings, str, boost::is_any_of("/"));
       result_type genValues;
       genValues.reserve(commandLine.m_dimension);
       for(const auto& matrixString : netDescriptionStrings)
       {
           std::vector<std::string> rowsStrings;
           boost::split(rowsStrings, matrixString, boost::is_any_of(","));
           if (rowsStrings.size()==0)
           {
               throw BadNetDescription("Bad matrix.");
           }
           for(const auto& rowString : rowsStrings)
           {
               if (rowString.size() != rowsStrings.front().size())
               {
                   throw BadNetDescription("Bad matrix (different row lengths).");
               }
           }
           GenValue genVal(rowsStrings.size(),rowsStrings.front().size());
           for(unsigned int i = 0; i < rowsStrings.size(); ++i)
           {
               std::reverse(rowsStrings[i].begin(), rowsStrings[i].end());
               genVal[i] = GeneratingMatrix::Row(rowsStrings[i]);
           }
           if(!NetConstructionTraits<NetConstruction::EXPLICIT>::checkGenValue(genVal, commandLine.m_designParameter))
           {
               throw BadNetDescription("Bad generating matrix size.");
           }
           genValues.push_back(std::move(genVal));
       }
        if (genValues.size() != commandLine.m_dimension )
        {
           throw BadNetDescription("Missing generating vector.");
        }
        return genValues;
   }
};

}}

#endif
