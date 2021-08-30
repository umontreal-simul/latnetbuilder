// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

template<LatticeType LR>
std::unique_ptr<typename LatBuilder::MeritFilterList<LR, EmbeddingType::MULTILEVEL>::Combiner>
MeritCombiner<LR>::parse(
      const std::string& str,
      const LatBuilder::SizeParam<LR, EmbeddingType::MULTILEVEL>& sizeParam
      )
{
   typedef std::unique_ptr<typename MeritFilterList<LR, EmbeddingType::MULTILEVEL>::Combiner> Ptr;
   const auto strSplit = splitPair<>(str, ':');
   if (strSplit.first == "level") {
      if (strSplit.second == "max")
         return Ptr(new LatBuilder::MeritCombiner::SelectLevel<LR>(sizeParam.maxLevel()));
      else {
         try {
            return Ptr(new LatBuilder::MeritCombiner::SelectLevel<LR>(boost::lexical_cast<Level>(strSplit.second)));
         }
         catch (boost::bad_lexical_cast&) {
            throw ParserError("cannot interpret \"" + strSplit.second + "\" as a level number");
         }
      }
   }

   if (str == "sum")
      return Ptr(new LatBuilder::MeritCombiner::Accumulator<LR, Functor::Sum>());
   if (str == "max")
      return Ptr(new LatBuilder::MeritCombiner::Accumulator<LR, Functor::Max>());

   throw BadCombiner(str);
}

template struct MeritCombiner<LatticeType::ORDINARY>;
template struct MeritCombiner<LatticeType::POLYNOMIAL>;

}}
