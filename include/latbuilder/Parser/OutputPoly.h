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
#include "latbuilder/DigitalNet/PolynomialLatticeBase2.h"

namespace LatBuilder { namespace Parser {

/**
 * Class for output-poly parameters
 * Defining parameters:
 * - doOutput wether to do the output or not;
 * - file file in which the output will be written
 * - outputFormat output format
 *
 * 
 */
class OutputPolyParameters {
public:
	OutputPolyParameters(bool doOutput, std::string file = "", LatBuilder::DigitalNet::OutputFormat outputFormat = LatBuilder::DigitalNet::OutputFormat::SSJ ):
		m_doOutput(doOutput),
      m_file(std::move(file)),
      m_outputFormat(outputFormat)

	{}

   bool doOutput() const {return m_doOutput; }
   std::string file() const {return m_file; }
    LatBuilder::DigitalNet::OutputFormat outputFormat() const {return m_outputFormat;}

private:
   bool m_doOutput;
   std::string m_file;
   LatBuilder::DigitalNet::OutputFormat m_outputFormat;
};

// parse output parameters
struct OutputPoly {
   
   static OutputPolyParameters parse(const std::string& str);
};


// parse ourput format
struct OutputFormat {
   
   static LatBuilder::DigitalNet::OutputFormat parse(const std::string& str);
};

//=============================================================

LatBuilder::DigitalNet::OutputFormat OutputFormat::parse(const std::string& str){
   if (str == "ssj" || str == "SSJ"){
      return LatBuilder::DigitalNet::OutputFormat::SSJ;
   }
   if (str == "web"){
      return LatBuilder::DigitalNet::OutputFormat::web_interface;
   }
   throw ParserError("cannot interpret \"" + str + "\" as an output format for polynomial lattices" );
}

OutputPolyParameters  OutputPoly::parse (const std::string& str){

   if (str == "") {return OutputPolyParameters(false);}

   std::vector< std::string> split = splitCSV(str);
   auto file_split = splitPair(split[0],':');
   

      LatBuilder::DigitalNet::OutputFormat outputFormat; 
      if (split.size() < 2 ) outputFormat = LatBuilder::DigitalNet::OutputFormat::SSJ;
      else outputFormat = OutputFormat::parse(splitPair(split[1],':').second);
      return OutputPolyParameters(true, file_split.second, outputFormat);
   

}

}}

#endif
