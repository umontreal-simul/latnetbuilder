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

#include "latbuilder/Parser/MeritCombiner.h"
#include "latbuilder/Functor/binary.h"
#include "latbuilder/MeritCombiner.h"

#include <boost/lexical_cast.hpp>

namespace LatBuilder { namespace Parser {

std::unique_ptr<typename LatBuilder::MeritFilterList<LatType::EMBEDDED>::Combiner>
MeritCombiner::parse(
      const std::string& str,
      const LatBuilder::SizeParam<LatType::EMBEDDED>& sizeParam
      )
{
   typedef std::unique_ptr<MeritFilterList<LatType::EMBEDDED>::Combiner> Ptr;
   const auto strSplit = splitPair<>(str, ':');
   if (strSplit.first == "level") {
      if (strSplit.second == "max")
         return Ptr(new LatBuilder::MeritCombiner::SelectLevel(sizeParam.maxLevel()));
      else {
         try {
            return Ptr(new LatBuilder::MeritCombiner::SelectLevel(boost::lexical_cast<Level>(strSplit.second)));
         }
         catch (boost::bad_lexical_cast&) {
            throw ParserError("cannot interpret \"" + strSplit.second + "\" as a level number");
         }
      }
   }

   if (str == "sum")
      return Ptr(new LatBuilder::MeritCombiner::Accumulator<Functor::Sum>());
   if (str == "max")
      return Ptr(new LatBuilder::MeritCombiner::Accumulator<Functor::Max>());

   throw BadCombiner(str);
}

}}
