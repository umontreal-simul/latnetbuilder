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
