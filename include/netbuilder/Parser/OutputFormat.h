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

#ifndef LATBUILDER__PARSER__OUTPUT_POLY_H
#define LATBUILDER__PARSER__OUTPUT_POLY_H

#include "latbuilder/Parser/Common.h"
#include "netbuilder/Types.h"

namespace NetBuilder
{
namespace Parser
{

namespace lbp = LatBuilder::Parser;
/**
 * Exception thrown when trying to parse an invalid output format.
 */
class BadOutputFormat : public lbp::ParserError
{
  public:
    BadOutputFormat(const std::string &message) : lbp::ParserError("cannot interpret \"" + message + "\" as an output format.")
    {
    }
};

/**
 * Class for output-poly parameters
 * Defining parameters:
 * - doOutput wether to do the output or not;
 * - file file in which the output will be written
 * - outputFormat output format
 *
 * 
 */
class OutputFormatParameters
{
    public:
      OutputFormatParameters(std::string file = "", OutputFormat outputFormat = OutputFormat::CLI) : 
            m_file(std::move(file)),
            m_outputFormat(outputFormat)
      {}

      std::string file() const { return m_file; }
      OutputFormat outputFormat() const { return m_outputFormat; }

    private:
      std::string m_file;
      OutputFormat m_outputFormat;
};

// parse output parameters
struct OutputFormatParser
{
      static std::vector<OutputFormatParameters> parse(const std::vector<string> &vec){
            std::vector<OutputFormatParameters> res; 
            for (auto str: vec){
                  std::vector<std::string> split = LatBuilder::Parser::splitCSV(str);
                  auto file_split = LatBuilder::Parser::splitPair(split[0], ':');

                  OutputFormat outputFormat;

                  if (split.size() < 2){
                        outputFormat = OutputFormat::CLI;
                  }
                  else{
                        std::string str = LatBuilder::Parser::splitPair(split[1], ':').second;
                        if (str == "gui" || str == "GUI"){
                              outputFormat =  OutputFormat::GUI;
                        }
                        else if (str == "cli" || str == "CLI"){
                              outputFormat =  OutputFormat::CLI;
                        }
                        else if (str == "ssj" || str == "SSJ"){
                              outputFormat =  OutputFormat::SSJ;
                        }
                        else{
                              throw BadOutputFormat(str);
                        }
                  }
                  res.push_back(OutputFormatParameters(file_split.second, outputFormat));
            }
            return res;
      }


};

} // namespace Parser
} // namespace NetBuilder

#endif
