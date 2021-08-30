// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/SizeParam.h"

#include "netbuilder/Parser/CommandLine.h"
#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"

#include <algorithm>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

namespace NetBuilder { namespace Parser {
namespace lbp = LatBuilder::Parser;
/**
 * Exception thrown when trying to parse an invalid net description.
 */
class BadNetDescription : public lbp::ParserError {
public:
   BadNetDescription(const std::string& message):
      lbp::ParserError("cannot parse net description string: " + message)
   {}
};

/**
 * Parser for net descriptions.
 */
template<NetConstruction NC, EmbeddingType ET>
struct NetDescriptionParser {};

template<EmbeddingType ET>
struct NetDescriptionParser<NetConstruction::SOBOL, ET>
{
    typedef typename NetConstructionTraits<NetConstruction::SOBOL>::GenValue GenValue;
   typedef std::vector<GenValue> result_type;

   static result_type parse(CommandLine<NetConstruction::SOBOL, ET>& commandLine, const std::string& str)
   {
       std::vector<std::string> netDescriptionStrings;
       boost::split(netDescriptionStrings, str, boost::is_any_of("-"));
       if (netDescriptionStrings.size() == commandLine.m_dimension - 1){
           // If there is one missing coordinate, we assume it is the first, and we add the direction number (always 0).
           netDescriptionStrings.insert(netDescriptionStrings.begin(), "0");
       }
       Dimension dim = 0;
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
           if(!NetConstructionTraits<NetConstruction::SOBOL>::checkGenValue(genVal, commandLine.m_sizeParameter))
           {
                throw BadNetDescription("bad Sobol' direction numbers.");
           }
           genValues.push_back(std::move(genVal));
           ++dim;
       }
        if (genValues.size() != commandLine.m_dimension)
        {
           throw BadNetDescription("incompatible dimension and Sobol' direction numbers");
        }
        return genValues;
   }
};

template<EmbeddingType ET>
struct NetDescriptionParser<NetConstruction::POLYNOMIAL, ET>
{
    typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::GenValue GenValue;
    typedef std::vector<GenValue> result_type;

   static result_type parse(CommandLine<NetConstruction::POLYNOMIAL, ET>& commandLine, const std::string& str)
   {
       std::vector<std::string> netDescriptionStrings;
       boost::split(netDescriptionStrings, str, boost::is_any_of("-"));
       result_type genValues;
       genValues.reserve(commandLine.m_dimension);
       for(const auto& polyString : netDescriptionStrings)
       {
           GenValue genVal = (GenValue) LatBuilder::Parser::SizeParam<LatBuilder::LatticeType::POLYNOMIAL, ET>::parse(polyString);
           if(!NetConstructionTraits<NetConstruction::POLYNOMIAL>::checkGenValue(genVal, commandLine.m_sizeParameter))
           {
               throw BadNetDescription("bad generating polynomial.");
           }
           genValues.push_back(std::move(genVal));
       }
        if (genValues.size() != commandLine.m_dimension )
        {
           throw BadNetDescription("incompatible dimension and generating vector.");
        }
        return genValues;
   }
};

template<EmbeddingType ET>
struct NetDescriptionParser<NetConstruction::EXPLICIT, ET>
{
    typedef typename NetConstructionTraits<NetConstruction::EXPLICIT>::GenValue GenValue;
    typedef std::vector<GenValue> result_type;

   static result_type parse(CommandLine<NetConstruction::EXPLICIT, ET>& commandLine, const std::string& str)
   {
       std::cout << str << std::endl;
       std::vector<std::string> netDescriptionStrings;
       boost::split(netDescriptionStrings, str, boost::is_any_of("-"));
       result_type genValues;
       genValues.reserve(commandLine.m_dimension);
       for(const auto& matrixString : netDescriptionStrings)
       {
           std::vector<std::string> columnsStrings;
           boost::split(columnsStrings, matrixString, boost::is_any_of(","));
           GeneratingMatrix genVal;
           try {
               std::vector<unsigned long> columns;
               columns.reserve(columnsStrings.size());
               for (const auto& columnString: columnsStrings){
                   columns.push_back(boost::lexical_cast<unsigned long>(columnString));
               }
                genVal = GeneratingMatrix::fromColsReverse(31, commandLine.m_sizeParameter.first, columns);
           }
           catch (std::exception& e) {
               throw BadNetDescription("Could not parse " + matrixString);
           }
           if(!NetConstructionTraits<NetConstruction::EXPLICIT>::checkGenValue(genVal, commandLine.m_sizeParameter))
           {
               throw BadNetDescription("bad generating matrix size.");
           }
           genValues.push_back(std::move(genVal));
       }
        if (genValues.size() != commandLine.m_dimension )
        {
           throw BadNetDescription("incompatible dimension and number of matrices.");
        }
        return genValues;
   }
};

template<EmbeddingType ET>
struct NetDescriptionParser<NetConstruction::LMS, ET>
{
    typedef typename NetConstructionTraits<NetConstruction::LMS>::GenValue GenValue;
    typedef std::vector<GenValue> result_type;

   static result_type parse(CommandLine<NetConstruction::LMS, ET>& commandLine, const std::string& str)
   {
       std::vector<std::string> netDescriptionStrings;
       boost::split(netDescriptionStrings, str, boost::is_any_of("-"));
       result_type genValues;
       genValues.reserve(commandLine.m_dimension);
       for(const auto& matrixString : netDescriptionStrings)
       {
           std::vector<std::string> rowsStrings;
           boost::split(rowsStrings, matrixString, boost::is_any_of(","));
           if (rowsStrings.size()==0)
           {
               throw BadNetDescription("bad matrix.");
           }
           for(const auto& rowString : rowsStrings)
           {
               if (rowString.size() != rowsStrings.front().size())
               {
                   throw BadNetDescription("bad matrix (different row lengths).");
               }
           }
           GenValue genVal((unsigned int) rowsStrings.size(),(unsigned int) rowsStrings.front().size());
           for(unsigned int i = 0; i < (unsigned int) rowsStrings.size(); ++i)
           {
               std::reverse(rowsStrings[i].begin(), rowsStrings[i].end());
               genVal[i] = GeneratingMatrix::Row(rowsStrings[i]);
           }
           if(!NetConstructionTraits<NetConstruction::LMS>::checkGenValue(genVal, commandLine.m_sizeParameter))
           {
               throw BadNetDescription("bad generating matrix size.");
           }
           genValues.push_back(std::move(genVal));
       }
        if (genValues.size() != commandLine.m_dimension )
        {
           throw BadNetDescription("incompatible dimension and number of matrices.");
        }
        return genValues;
   }
};

}}

#endif
