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

#include "latbuilder/Parser/MeritFilterList.h"
#include "latbuilder/Parser/MeritCombiner.h"
#include "latbuilder/Parser/MeritFilter.h"
#include "latbuilder/MeritFilterList.h"
#include "latbuilder/SizeParam.h"

#include "latticetester/Weights.h"

namespace LatBuilder { namespace Parser {

template <LatticeType LR>
void MeritFilterList<LR>::parse(
      LatBuilder::MeritFilterList<LR, EmbeddingType::UNILEVEL>& list,
      const std::string& figure,
      const std::vector<std::string>& filters,
      const LatBuilder::SizeParam<LR, EmbeddingType::UNILEVEL>& sizeParam,
      const LatticeTester::Weights& weights,
      Real normType
      )
{
   for (const auto& arg : filters)
      list.add(MeritFilter<LR, EmbeddingType::UNILEVEL>::parse(arg, figure, sizeParam, weights, normType));
}

template <LatticeType LR>
void MeritFilterList<LR>::parse(
      LatBuilder::MeritFilterList<LR, EmbeddingType::MULTILEVEL>& list,
      const std::string& figure,
      const std::vector<std::string>& filters,
      const std::string& combiner,
      const LatBuilder::SizeParam<LR, EmbeddingType::MULTILEVEL>& sizeParam,
      const LatticeTester::Weights& weights,
      Real normType
      )
{
   for (const auto& arg : filters)
      list.add(MeritFilter<LR, EmbeddingType::MULTILEVEL>::parse(arg, figure, sizeParam, weights, normType, combiner));

   list.add(MeritCombiner<LR>::parse(combiner, sizeParam));
}

template struct MeritFilterList<LatticeType::ORDINARY> ;
template struct MeritFilterList<LatticeType::POLYNOMIAL> ;


}}
