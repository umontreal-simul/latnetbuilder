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
template<LatBuilder::LatticeType LA>
void simulate(const LatticePoints<LA>& lat)
{
   for (size_t i = 0; i < lat.size(); i++)
      std::cout << "point " << i << ":\t" << lat[i] << std::endl;
}
//! [simulate]

//! [search]
template<LatBuilder::LatticeType LA>
LatticePoints<LA> search()
{
   LatBuilder::Parser::CommandLine<LA, LatBuilder::EmbeddingType::UNILEVEL> cmd;
   cmd.construction  = "fast-CBC";
   cmd.size          = "2^8"; // for example "1011" for polynomial lattice
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
   return LatticePoints<LA>(lat.sizeParam().modulus(), lat.gen());
}
//! [search]

int main(int argc, const char *argv[])
{
   LatticePoints<LatBuilder::LatticeType::ORDINARY> lat = search<LatBuilder::LatticeType::ORDINARY>();
   simulate<LatBuilder::LatticeType::ORDINARY>(lat);
   return 0;
}