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

#include "latbuilder/Parser/CommandLine.h"
#include "latbuilder/TextStream.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

LatBuilder::GeneratingVector searchLattice(
      LatBuilder::Modulus numPoints,
      LatBuilder::Dimension dimension
      )
{
   LatBuilder::Parser::CommandLine<LatBuilder::LatType::ORDINARY> cmd;
   cmd.construction  = "fast-CBC";
   cmd.size          = boost::lexical_cast<std::string>(numPoints);
   cmd.dimension     = boost::lexical_cast<std::string>(dimension);
   cmd.figure        = "CU:P2";
   cmd.weights       = std::vector<std::string>{"product:0.1"};
   auto search = cmd.parse();
   search->execute();
   return search->bestLattice().gen();
}

int main(int argc, const char *argv[])
{
   using LatBuilder::TextStream::operator<<;
   LatBuilder::Modulus n = 65537;
   LatBuilder::Dimension dim = 10;
   LatBuilder::GeneratingVector gen = searchLattice(n, dim);
   std::cout << "number of points:  " << n << std::endl;
   std::cout << "dimension:         " << dim << std::endl;
   std::cout << "generating vector: " << gen << std::endl;
   return 0;
}
