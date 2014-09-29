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
#include "LatticePoints.h"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const std::vector<double>& point)
{
   auto old_prec = os.precision();
   os.precision(3);
   os << "(";
   for (size_t j = 0; j < point.size(); j++) {
      if (j > 0) os << ", ";
      os << point[j];
   }
   os << ")";
   os.precision(old_prec);
   return os;
}

//! [simulate]
void simulate(const LatticePoints& lat)
{
   for (size_t i = 0; i < lat.size(); i++)
      std::cout << "point " << i << ":\t" << lat[i] << std::endl;
}
//! [simulate]

//! [search]
LatticePoints search()
{
   LatBuilder::Parser::CommandLine<LatBuilder::LatType::ORDINARY> cmd;
   cmd.construction  = "fast-CBC";
   cmd.size          = "2^8";
   cmd.dimension     = "10";
   cmd.figure        = "CS:P2";
   cmd.weights       = std::vector<std::string>{"product:0.1"};
   cmd.normType      = "2";
   auto search = cmd.parse();

   std::cout << *search << std::endl;
   search->execute();
   std::cout << "BEST LATTICE: " << search->bestLattice() << std::endl;
   std::cout << "MERIT:        " << search->bestMeritValue() << std::endl;

   const auto& lat = search->bestLattice();
   return LatticePoints(lat.sizeParam().numPoints(), lat.gen());
}
//! [search]

int main(int argc, const char *argv[])
{
   LatticePoints lat = search();
   simulate(lat);
   return 0;
}
