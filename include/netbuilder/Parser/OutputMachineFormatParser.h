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

#ifndef NETBUILDER__PARSER__OUTPUT_MACHINE_FORMAT_PARSER_H
#define NETBUILDER__PARSER__OUTPUT_MACHINE_FORMAT_PARSER_H

#include "netbuilder/Types.h"

namespace NetBuilder { namespace Parser {
/**
 * Exception thrown when trying to parse an invalid machine format.
 */
class BadMachineFormat : public std::exception {
public:
   BadMachineFormat(const std::string& message, NetConstruction netConstruction){
      GetDefaultMachineFormat(netConstruction);
   }
   const char* what() { return message.c_str(); } //message of warning
private:
   std::string message;

   OutputMachineFormat GetDefaultMachineFormat(NetConstruction netConstruction)
   {
      if(netConstruction == NetConstruction::SOBOL)
      {
        return NetBuilder::OutputMachineFormat::SOBOL;
      }
      else if(netConstruction == NetConstruction::POLYNOMIAL)
      {
        return NetBuilder::OutputMachineFormat::POLYNOME;
      }
      else
      {
        return NetBuilder::OutputMachineFormat::EXPLICIT;
      }
   }  
};

/**
 * Parser for machine formats.
 */
template <NetConstruction NC>
struct OutputMachineFormatParser {};

template<>
struct OutputMachineFormatParser<NetConstruction::SOBOL>
{
   typedef NetBuilder::OutputMachineFormat result_type;

   static result_type parse(const std::string& str)
   {
      if (str == "sobol")
      {
        return NetBuilder::OutputMachineFormat::SOBOL;
      }
      else if (str == "soboljk")
      {
        return NetBuilder::OutputMachineFormat::SOBOLJK;
      }
      else if (str == "net")
      {
        return NetBuilder::OutputMachineFormat::DIGITALNET;
      }
      else if (str == "")
      {
        return NetBuilder::OutputMachineFormat::NONE;
      }
      else
      {
        std::cout << "WARNING: cannot parse output machine format string. Default output machine format: SOBOL" << std::endl;
        return NetBuilder::OutputMachineFormat::SOBOL;
      }
   }
};

template<>
struct OutputMachineFormatParser<NetConstruction::POLYNOMIAL>
{
   typedef NetBuilder::OutputMachineFormat result_type;

   static result_type parse(const std::string& str)
   {
      if (str == "lattice")
      {
        return NetBuilder::OutputMachineFormat::POLYNOME;
      }
      else if (str == "net")
      {
        return NetBuilder::OutputMachineFormat::DIGITALNET;
      }
      else if (str == "")
      {
        return NetBuilder::OutputMachineFormat::NONE;
      }
      else
      {
        std::cout << "WARNING: cannot parse output machine format string. Default output machine format: POLYNOME" << std::endl;
        return NetBuilder::OutputMachineFormat::POLYNOME;
      }
   }
};

template<>
struct OutputMachineFormatParser<NetConstruction::EXPLICIT>
{
   typedef NetBuilder::OutputMachineFormat result_type;

   static result_type parse(const std::string& str)
   {
      if (str == "net")
      {
        return NetBuilder::OutputMachineFormat::EXPLICIT;
      }
      else if (str == "")
      {
        return NetBuilder::OutputMachineFormat::NONE;
      }
      else
      {
        std::cout << "WARNING: cannot parse output machine format string. Default output machine format: EXPLICIT" << std::endl;
        return NetBuilder::OutputMachineFormat::EXPLICIT;
      }
   }
};

}}
#endif
