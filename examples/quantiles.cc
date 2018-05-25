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

#include "latbuilder/Parser/SizeParam.h"
#include "latbuilder/Parser/CoordUniformFigureOfMerit.h"
#include "latbuilder/Parser/Weights.h"
#include "latbuilder/Parser/MeritFilterList.h"

#include "latbuilder/MeritSeq/LatSeqOverCBC.h"
#include "latbuilder/MeritSeq/CoordUniformCBC.h"
#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/Traversal.h"
#include "latbuilder/LFSR113.h"
#include "latbuilder/TextStream.h"

#include <iostream>
#include <iomanip>
#include <functional>
#include <vector>

#include <boost/ref.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/tail_quantile.hpp>

using namespace LatBuilder;
using TextStream::operator<<;

template <typename T1, typename T2>
void printTableRow(const T1& x1, const T2& x2)
{
   using namespace std;
   cout << fixed << setprecision(3) << x1 << '\t'
      << scientific << setprecision(8) << x2 << std::endl;
}

struct Execute {
   template <class FIGURE, class LATSEQ>
   void execute(
         FIGURE figure,
         LatBuilder::SizeParam<Lattice::INTEGRATION, LatType::ORDINARY> size,
         LATSEQ latSeq,
         unsigned int numSamples
         ) const
   {
      Storage<Lattice::INTEGRATION, LatType::ORDINARY, FIGURE::suggestedCompression()> storage(std::move(size));

      auto latSeqOverCBC = MeritSeq::latSeqOverCBC(MeritSeq::cbc(storage, figure));

      //! [accumulator setup]
      using namespace boost::accumulators;

      accumulator_set<Real, features<tag::count, tag::min, tag::max, tag::mean, tag::tail_quantile<left>>>
          acc(tag::tail<left>::cache_size = numSamples);

      auto meritSeq = latSeqOverCBC.meritSeq(latSeq);
      for (const auto& val : meritSeq)
         acc(val);
      //! [accumulator setup]

      //! [output]
      unsigned int numBins = 20;
      printTableRow("# mean:", mean(acc));
      printTableRow("prob", "quantile");
      printTableRow(0.0, min(acc));
      for (unsigned int i = 1; i < numBins; i++) {
          double p = double(i) / numBins;
          Real q = quantile(acc, quantile_probability = p);
          printTableRow(p, q);
      }
      printTableRow(1.0, max(acc));
      //! [output]
   }

   // random sampling
   template <class FIGURE>
   void operator()(FIGURE figure, LatBuilder::SizeParam<Lattice::INTEGRATION, LatType::ORDINARY> size, Dimension dimension, unsigned int nrand) const
   {
      typedef GenSeq::GeneratingValues<Lattice::INTEGRATION, FIGURE::suggestedCompression(), Traversal::Random<LFSR113>> Coprime;
      auto genSeqs = GenSeq::VectorCreator<Coprime>::create(size, dimension, nrand);
      genSeqs[0] = GenSeq::Creator<Coprime>::create(SizeParam<Lattice::INTEGRATION, LatType::ORDINARY>(2), nrand);

      auto latSeq = LatSeq::combine<Zip>(size, std::move(genSeqs));

      std::cout << "# random samples: " << nrand << std::endl;

      execute(
            std::move(figure),
            std::move(size),
            std::move(latSeq),
            nrand
            );
   }

   // exhaustive sampling
   template <class FIGURE>
   void operator()(FIGURE figure, LatBuilder::SizeParam<Lattice::INTEGRATION, LatType::ORDINARY> size, Dimension dimension) const
   {
      typedef GenSeq::GeneratingValues<Lattice::INTEGRATION, FIGURE::suggestedCompression()> Coprime;
      auto genSeqs = GenSeq::VectorCreator<Coprime>::create(size, dimension);
      genSeqs[0] = GenSeq::Creator<Coprime>::create(SizeParam<LatType::ORDINARY>(2));

      unsigned int numSamples = 1;
      for (const auto& g : genSeqs)
         numSamples *= g.size();

      auto latSeq = LatSeq::combine<CartesianProduct>(size, std::move(genSeqs));

      std::cout << "# deterministic samples: " << numSamples << std::endl;

      execute(
            std::move(figure),
            std::move(size),
            std::move(latSeq),
            numSamples
            );
   }
};

int main(int argc, const char *argv[])
{
   try {
      if (argc < 4 + 1 || argc > 5 + 1) {
         std::cout << "usage: quantiles <size> <dimension> <figure-of-merit> <weights> [<random samples>]" << std::endl;
         std::cout << "Do not use the prefix `CU:' on <figure-of-merit>; it will be automatically added." << std::endl;
         return -1;
      }

      int iarg = 0;
      std::string sizeSpec = argv[++iarg];
      Dimension dimension = (Dimension)atoi(argv[++iarg]);
      std::string figureSpec = argv[++iarg];
      std::string weightsSpec = argv[++iarg];
      auto weights = Parser::Weights::parse(weightsSpec);
      unsigned int nrand = 0;
      if (++iarg < argc)
        nrand =  (unsigned int)atoi(argv[iarg]);

      auto size = Parser::SizeParam::parse<Lattice::INTEGRATION, LatType::ORDINARY>(sizeSpec);

      if (nrand) {
         Parser::CoordUniformFigureOfMerit::parse(
               "2",
               figureSpec,
               std::move(weights),
               Execute(),
               size,
               dimension,
               nrand);
      }
      else {
         Parser::CoordUniformFigureOfMerit::parse(
               "2",
               figureSpec,
               std::move(weights),
               Execute(),
               size,
               dimension);
      }
   }
   catch (Parser::ParserError& e) {
      std::cerr << "COMMAND LINE ERROR: " << e.what() << std::endl;
      std::exit(1);
   }
   catch (std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      std::exit(1);
   }

   return 0;
}
