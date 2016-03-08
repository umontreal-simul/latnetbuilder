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
#include "latbuilder/Parser/Common.h"
#include "latbuilder/Util.h"

namespace LatBuilder { namespace Parser {

template <>
LatBuilder::SizeParam<LatBuilder::LatType::ORDINARY>
SizeParam::parse<LatBuilder::LatType::ORDINARY>(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<Modulus, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatBuilder::LatType::ORDINARY>(n.first);
   else
      return LatBuilder::SizeParam<LatBuilder::LatType::ORDINARY>(intPow(n.first, n.second));
}

template <>
LatBuilder::SizeParam<LatBuilder::LatType::EMBEDDED>
SizeParam::parse<LatBuilder::LatType::EMBEDDED>(const std::string& str)
{
   // try b^p form first
   auto n = splitPair<Modulus, Level>(str, '^', 0);
   if (n.second == 0)
      return LatBuilder::SizeParam<LatBuilder::LatType::EMBEDDED>(n.first);
   else
      return LatBuilder::SizeParam<LatBuilder::LatType::EMBEDDED>(n.first, n.second);
}

extern template LatBuilder::SizeParam<LatType::ORDINARY> SizeParam::parse<LatType::ORDINARY>(const std::string&);
extern template LatBuilder::SizeParam<LatType::EMBEDDED> SizeParam::parse<LatType::EMBEDDED>(const std::string&);

}}
