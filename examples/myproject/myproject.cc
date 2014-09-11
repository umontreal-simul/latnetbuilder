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
   cmd.figure        = "CS:P2";
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
