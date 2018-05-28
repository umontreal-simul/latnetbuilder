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
#include "latbuilder/Parser/Weights.h"
#include "latbuilder/Parser/FigureOfMerit.h"
#include "latbuilder/Parser/MeritFilterList.h"
#include "latbuilder/Types.h"
#include "latbuilder/TextStream.h"
#include "latbuilder/LFSR113.h"

#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/GenSeq/CoprimeIntegers.h"
#include "latbuilder/LatSeq/Combiner.h"

#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/MeritSeq/CoordUniformCBC.h"
#include "latbuilder/MeritSeq/CoordUniformInnerProd.h"
#include "latbuilder/MeritSeq/LatSeqOverCBC.h"

#include <boost/lexical_cast.hpp>

using namespace LatBuilder;
using TextStream::operator<<;

template <typename T1, typename T2, typename T3, typename T4>
void printTableRow(
      const T1& x1,
      const T2& x2,
      const T3& x3,
      const T4& x4
      )
{
   using namespace std;
   cout << x1 << '\t';
   cout << scientific << setprecision(8) << x2 << '\t';
   cout << scientific << setprecision(8) << x3 << '\t';
   cout << scientific << setprecision(8) << x4 << endl;;
}

struct Execute {
   template <class FIG2>
   void operator()(
         FIG2 fig2,
         const std::string& fig1,
         std::unique_ptr<LatticeTester::Weights> weights1,
         LatBuilder::SizeParam<LatType::ORDINARY> size,
         Dimension dimension,
         MeritFilterList<LatType::ORDINARY> filters,
         size_t nrand
         ) const
   {
      Parser::FigureOfMerit::parse(
	    "2",
            fig1,
            std::move(weights1),
            *this,
            std::move(fig2),
            std::move(size),
            dimension,
            std::move(filters),
            nrand
            );
   }

   template <class FIG1, class FIG2>
   void operator()(
         FIG1 fig1,
         FIG2 fig2,
         LatBuilder::SizeParam<LatType::ORDINARY> size,
         Dimension dimension,
         MeritFilterList<LatType::ORDINARY> filters,
         size_t nrand
         ) const
   {
      Storage<LatType::ORDINARY, fig1.suggestedCompression()> storage1(size);
      Storage<LatType::ORDINARY, fig2.suggestedCompression()> storage2(size);

      typedef GenSeq::CoprimeIntegers<fig1.suggestedCompression(), Traversal::Random<LFSR113>> Coprime;
      auto genSeqs = GenSeq::VectorCreator<Coprime>::create(size, dimension, nrand);
      genSeqs[0] = GenSeq::Creator<Coprime>::create(SizeParam<LatType::ORDINARY>(2));

      auto latSeq = LatSeq::combine<Zip>(size, std::move(genSeqs));

      auto lsoc1 = MeritSeq::latSeqOverCBC(MeritSeq::cbc(storage1, fig1));
      auto lsoc2 = MeritSeq::latSeqOverCBC(MeritSeq::cbc(storage2, fig2));

      auto fseq1 = filters.apply(lsoc1.meritSeq(latSeq));
      auto fseq2 = filters.apply(lsoc2.meritSeq(latSeq));

      printTableRow("gen", fig1.name(), fig2.name(),
            "rel" + fig1.name() + "m" + fig2.name());
      
      auto it1 = fseq1.begin();
      auto it2 = fseq2.begin();
      while (it1 != fseq1.end()) {
         printTableRow(
               it1.base().base()->gen(),
               *it1,
               *it2,
               (*it1 - *it2) / *it1
               );
         ++it1;
         ++it2;
      }

   }
};

int main(int argc, const char *argv[])
{
   if (argc < 7 + 1) {
      std::cerr << "usage: correlation <nrand> <size> <dimension> <figure1> <weights1> <figure2> <weights2> [<filter> [...]]" << std::endl;
      return 1;
   }

   int iarg = 1;
   auto nrand = boost::lexical_cast<size_t>(argv[iarg++]);
   auto size = Parser::SizeParam::parse<LatType::ORDINARY>(argv[iarg++]);
   auto dimension = boost::lexical_cast<Dimension>(argv[iarg++]);
   std::string figSpec1 = argv[iarg++];
   std::string weightsSpec1 = argv[iarg++];
   std::string figSpec2 = argv[iarg++];
   std::string weightsSpec2 = argv[iarg++];
   std::vector<std::string> filtersSpec(&argv[iarg], &argv[argc]);

   auto weights1 = Parser::Weights::parse(weightsSpec1);
   auto weights2 = Parser::Weights::parse(weightsSpec2);

   auto filters = Parser::MeritFilterList::parse(filtersSpec, size, *weights1, 2);

   Parser::FigureOfMerit::parse(
         "2",
         figSpec2,
         std::move(weights2),
         Execute(),
         figSpec1,
         std::move(weights1),
         std::move(size),
         dimension,
         std::move(filters),
         nrand
         );

   return 0;
}
