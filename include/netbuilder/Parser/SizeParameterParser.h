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

#ifndef NETBUILDER__PARSER__SIZE_PARAMETER_PARSER_H
#define NETBUILDER__PARSER__SIZE_PARAMETER_PARSER_H

#include "latbuilder/Util.h"
#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/SizeParam.h"

#include "netbuilder/Types.h"
#include "netbuilder/Helpers/JoeKuo.h"
#include "netbuilder/GeneratingMatrix.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/Parser/CommandLine.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace NetBuilder { namespace Parser {
namespace lbp = LatBuilder::Parser;
/**
 * Exception thrown when trying to parse an invalid size parameter.
 */
class BadSizeParameter : public lbp::ParserError {
public:
   BadSizeParameter(const std::string& message):
      lbp::ParserError("cannot parse size parameter string: " + message)
   {}
};

/**
 * Parser for size parameters.
 */
template<NetConstruction NC, EmbeddingType ET>
struct SizeParameterParser {};

template<EmbeddingType ET>
struct SizeParameterParser<NetConstruction::SOBOL, ET>
{
   typedef typename NetConstructionTraits<NetConstruction::SOBOL>::SizeParameter result_type;

   static void parse(Parser::CommandLine<NetConstruction::SOBOL, ET>& commandLine)
   {
         std::vector<std::string> sizeParamStrings;
         boost::split(sizeParamStrings, commandLine.s_size, boost::is_any_of("^"));
         if (sizeParamStrings.size() == 2 && sizeParamStrings.front() == "2")
         {
               unsigned int nCols = boost::lexical_cast<unsigned int>(sizeParamStrings.back());
               commandLine.m_sizeParameter = result_type(nCols);
         }
         else
         {
            throw BadSizeParameter(commandLine.s_size);
         }
   }
};

template<EmbeddingType ET>
struct SizeParameterParser<NetConstruction::POLYNOMIAL, ET>
{
   typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::SizeParameter result_type;

   static void parse(Parser::CommandLine<NetConstruction::POLYNOMIAL, ET>& commandLine)
   {
      commandLine.m_sizeParameter = (NetConstructionTraits<NetConstruction::POLYNOMIAL>::SizeParameter) LatBuilder::Parser::SizeParam<LatBuilder::LatticeType::POLYNOMIAL, ET>::parse(commandLine.s_size);
   }
};

template<EmbeddingType ET>
struct SizeParameterParser<NetConstruction::EXPLICIT, ET>
{
      typedef typename NetConstructionTraits<NetConstruction::EXPLICIT>::SizeParameter result_type;

      static void parse(Parser::CommandLine<NetConstruction::EXPLICIT, ET>& commandLine)
      {
         std::vector<std::string> sizeParamStrings;
         boost::split(sizeParamStrings, commandLine.s_size, boost::is_any_of("^"));
         if (sizeParamStrings.size() == 2 && sizeParamStrings.front() == "2")
         {
               unsigned int nCols = boost::lexical_cast<unsigned int>(sizeParamStrings.back());
               commandLine.m_sizeParameter = result_type(nCols,nCols);
         }
         else
         {
            throw BadSizeParameter(commandLine.s_size);
         }
      }
};

template<EmbeddingType ET>
struct SizeParameterParser<NetConstruction::LMS, ET>
{
      typedef typename NetConstructionTraits<NetConstruction::LMS>::SizeParameter result_type;

      static void parse(Parser::CommandLine<NetConstruction::LMS, ET>& commandLine)
      {
         std::vector<std::string> sizeParamStrings;
         boost::split(sizeParamStrings, commandLine.s_size, boost::is_any_of("^"));
         if (sizeParamStrings.size() == 2 && sizeParamStrings.front() == "2")
         {
               unsigned int nCols = boost::lexical_cast<unsigned int>(sizeParamStrings.back());
               std::vector<JoeKuo::DirectionNumbers> joeKuoDirectionNumbers = JoeKuo::getJoeKuoDirectionNumbers(commandLine.m_dimension);
               Dimension dimension_j = 0; //index of the dimension of the net, used for creating JoeKuo net 
               std::vector<std::shared_ptr<GeneratingMatrix>> baseNet;
               for(const auto& genValue : joeKuoDirectionNumbers)
               {
                  // construct the generating matrix and store them
                  baseNet.push_back(std::shared_ptr<GeneratingMatrix>(NetConstructionTraits<NetConstruction::SOBOL>::createGeneratingMatrix(genValue, nCols, dimension_j)));
                  dimension_j++;
               }
               
               commandLine.m_sizeParameter = result_type(std::pair<unsigned int, unsigned int>(nCols, nCols), baseNet);
         }
         else
         {
            throw BadSizeParameter(commandLine.s_size);
         }
      }
};

}}

#endif
