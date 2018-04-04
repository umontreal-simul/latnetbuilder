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
#include "EmbeddedLatticePoints.h"

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
template<LatBuilder::LatticeType LA>
void simulate(EmbeddedLatticePoints<LA> lat)
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
template<LatBuilder::LatticeType LA>
EmbeddedLatticePoints<LA> search()
{
   //! [search inst]
   LatBuilder::Parser::CommandLine<LA, LatBuilder::LatEmbed::EMBEDDED> cmd;
   cmd.construction  = "fast-CBC";
   cmd.size          = "2^8";
   cmd.dimension     = "10";
   cmd.figure        = "CU:P2";
   cmd.weights       = std::vector<std::string>{"product:0.1"}; 
   cmd.weightsPowerScale = 1.0;
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
   return EmbeddedLatticePoints<LA>(lat.sizeParam().base(), lat.sizeParam().maxLevel(), lat.gen());
   //! [return lattice]
}
//! [search]

int main(int argc, const char *argv[])
{
   EmbeddedLatticePoints<LatBuilder::LatticeType::ORDINARY> lat = search<LatBuilder::LatticeType::ORDINARY>();
   simulate<LatBuilder::LatticeType::ORDINARY>(lat);
   return 0;
}
