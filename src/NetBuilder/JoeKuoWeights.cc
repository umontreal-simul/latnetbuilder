// This file is part of LatCommon.
//
// LatCommon
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

#include "netbuilder/JoeKuoWeights.h"
#include "netbuilder/Util.h"
#include <cmath>

using namespace std;

namespace NetBuilder
{

//===========================================================================

JoeKuoWeights::JoeKuoWeights():
   Weights()
{}

//===========================================================================

Weight JoeKuoWeights::getWeight (const Coordinates& projection) const
{
   if (projection.size()==2)
   {
         return std::pow((Weight).9999, *projection.rbegin());
   }
   // fall back to zero
   return 0.0;
}

//===========================================================================

void JoeKuoWeights::format(ostream& os) const
{
   using LatCommon::operator<<;
   os << "JoeKuoWeights()";
}

} // namespace
