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

// Example of how to use the LFSR113 generator with the facilities of the C++11
// standard.

#include "latbuilder/LFSR113.h"
#include <random>
#include <functional>
#include <iostream>
#include <iomanip>

int main(int argc, const char *argv[])
{
   const unsigned int min = 1;
   const unsigned int max = 20;
   const size_t njumps = 5;
   const size_t nsamples = 1000000lu;

   LatBuilder::LFSR113 generator;
   std::uniform_int_distribution<unsigned int> dist(min, max);

   // build histogram
   std::vector<std::vector<unsigned int>> counts(max - min + 1);

   for (size_t ijump = 0; ijump < njumps; ijump++) {

      // calling "randInt()" is equivalent to calling "dist(generator)"
      std::function<unsigned int()> randInt = std::bind(dist, generator);

      counts[ijump].resize(max - min + 1);

      for (size_t i = 0; i < nsamples; i++)
         counts[ijump][randInt() - min]++;

      generator.jump();
   }

   // output histogram
   std::cout.precision(1);
   for (unsigned int x = min; x <= max; x++) {
      std::cout << "count(" << std::setw(2) << std::right << x << "):";
      for (size_t ijump = 0; ijump < njumps; ijump++)
         std::cout << std::setw(8) << std::right << std::fixed << (100 * (double)counts[ijump][x - min] / nsamples) << " %";
      std::cout << std::endl;
   }

   return 0;
}
