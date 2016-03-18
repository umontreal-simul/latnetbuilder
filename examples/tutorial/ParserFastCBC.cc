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
   cmd.figure        = "CU:P2";
   cmd.weights       = std::vector<std::string>{"product:0.1"};
   cmd.weightsPowerScale = 1.0;
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
