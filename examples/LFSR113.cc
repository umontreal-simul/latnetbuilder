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
