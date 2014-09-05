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
#include "./EmbeddedLatticePoints.h"

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
void simulate(EmbeddedLatticePoints lat)
{
   while (lat.level() <= lat.maxLevel()) {
      std::cout << "==> level " << lat.level() << std::endl;
      for (size_t i = 0; i < lat.size(); i++)
         std::cout << "point " << i << ":\t" << lat[i] << std::endl;
      lat.extend();
   }
}
//! [simulate]

//! [search]
EmbeddedLatticePoints search()
{
   //! [search inst]
   LatBuilder::Parser::CommandLine<LatBuilder::LatType::EMBEDDED> cmd;
   cmd.construction  = "fast-CBC";
   cmd.size          = "2^8";
   cmd.dimension     = "10";
   cmd.figure        = "CS:P2";
   cmd.weights       = std::vector<std::string>{"product:0.1"};
   cmd.normType      = "2";
   //! [filters]
   cmd.multilevelFilters = std::vector<std::string>{"norm:P2-SL10", "low-pass:1.0"};
   //! [filters]
   cmd.combiner      = "sum";
   auto search = cmd.parse();
   //! [search inst]

   std::cout << *search << std::endl;
   search->execute();
   std::cout << "BEST LATTICE: " << search->bestLattice() << std::endl;
   std::cout << "MERIT:        " << search->bestMeritValue() << std::endl;

   //! [return lattice]
   const auto& lat = search->bestLattice();
   return EmbeddedLatticePoints(lat.sizeParam().base(), lat.sizeParam().maxLevel(), lat.gen());
   //! [return lattice]
}
//! [search]

int main(int argc, const char *argv[])
{
   EmbeddedLatticePoints lat = search();
   simulate(lat);
   return 0;
}
