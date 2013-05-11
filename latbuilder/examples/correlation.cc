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
#include "latbuilder/MeritSeq/CoordSymCBC.h"
#include "latbuilder/MeritSeq/CoordSymInnerProd.h"
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
         std::unique_ptr<LatCommon::Weights> weights1,
         LatBuilder::SizeParam<LatType::ORDINARY> size,
         Dimension dimension,
         MeritFilterList<LatType::ORDINARY> filters,
         size_t nrand
         ) const
   {
      Parser::FigureOfMerit::parse(
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

   auto filters = Parser::MeritFilterList::parse(filtersSpec, size, *weights1);

   Parser::FigureOfMerit::parse(
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
