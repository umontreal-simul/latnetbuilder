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

#include "latbuilder/Parser/LevelWeights.h"

namespace LatBuilder { namespace Parser {


template <LatticeType LR> 
auto LevelWeights<LR>::parse(
      const std::string& arg,
      const SizeParam<LR, EmbeddingType::MULTILEVEL>& sizeParam
      ) -> result_type
{
   const auto args = splitPair<>(arg, ':');
   if (arg.empty() or args.first == "select") {
      Level minLevel = 1;
      Level maxLevel = sizeParam.maxLevel();
      if (not args.second.empty()) {
         // parse min/max levels
         const auto levels = splitPair<Level, Level>(args.second, ',', maxLevel);
         minLevel = levels.first;
         maxLevel = std::min(levels.second, sizeParam.maxLevel());
      }
      RealVector weights(sizeParam.maxLevel() + 1, 0.0);
      for (Level level = minLevel; level <= maxLevel; level++)
         weights[level] = 1.0 / (maxLevel - minLevel + 1);
      return weights;
   }
   throw BadLevelWeights(arg);
}

template struct LevelWeights<LatticeType::ORDINARY>;
template struct LevelWeights<LatticeType::POLYNOMIAL>;

}}
