// Copyright (c) 2012, 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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

#include "latbuilder/Parser/SizeParam.h"
#include "latbuilder/Parser/CoordSymFigureOfMerit.h"
#include "latbuilder/Parser/Weights.h"
#include "latbuilder/Parser/MeritFilterList.h"

#include "latbuilder/MeritSeq/LatSeqOverCBC.h"
#include "latbuilder/MeritSeq/CoordSymCBC.h"
#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
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
         LatBuilder::SizeParam<LatType::ORDINARY> size,
         LATSEQ latSeq,
         unsigned int numSamples
         ) const
   {
      Storage<LatType::ORDINARY, FIGURE::suggestedCompression()> storage(std::move(size));

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
   void operator()(FIGURE figure, LatBuilder::SizeParam<LatType::ORDINARY> size, Dimension dimension, unsigned int nrand) const
   {
      typedef GenSeq::CoprimeIntegers<FIGURE::suggestedCompression(), Traversal::Random<LFSR113>> Coprime;
      auto genSeqs = GenSeq::VectorCreator<Coprime>::create(size, dimension, nrand);
      genSeqs[0] = GenSeq::Creator<Coprime>::create(SizeParam<LatType::ORDINARY>(2), nrand);

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
   void operator()(FIGURE figure, LatBuilder::SizeParam<LatType::ORDINARY> size, Dimension dimension) const
   {
      typedef GenSeq::CoprimeIntegers<FIGURE::suggestedCompression()> Coprime;
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
         std::cout << "usage: quantiles <size> <dimension> <figure-or-merit> <weights> [<random samples>]" << std::endl; // **XX Pls specify that the user should not type in the CS: but that this is assumed...
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

      auto size = Parser::SizeParam::parse<LatType::ORDINARY>(sizeSpec);

      if (nrand) {
         Parser::CoordSymFigureOfMerit::parse(
               "2",
               figureSpec,
               std::move(weights),
               Execute(),
               size,
               dimension,
               nrand);
      }
      else {
         Parser::CoordSymFigureOfMerit::parse(
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
