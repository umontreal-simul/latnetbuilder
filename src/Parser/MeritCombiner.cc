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
