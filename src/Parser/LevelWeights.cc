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

#include "latbuilder/Parser/LevelWeights.h"

namespace LatBuilder { namespace Parser {

auto LevelWeights::parse(
      const std::string& arg,
      const SizeParam<LatType::EMBEDDED>& sizeParam
      ) -> result_type
{
   const auto args = splitPair<>(arg, ':');
   if (arg.empty() or args.first == "even") {
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

}}
