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

#include "latbuilder/LatDef.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/TextStream.h"
#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

//! [printLatDef]
template <LatType L>
void printLatDef(const LatDef<L>& def)
{
   std::cout << "    dimension:         " << def.dimension() << std::endl;
   std::cout << "    size parameter:    " << def.sizeParam() << std::endl;
   std::cout << "    generating vector: " << def.gen() << std::endl;
   std::cout << "    definition:        " << def << std::endl;
}
//! [printLatDef]

int main()
{
   //! [ordinary]
   auto ordinary = createLatDef(SizeParam<LatType::ORDINARY>(31), {1, 12, 3});
   std::cout << "ordinary lattice:" << std::endl;
   printLatDef(ordinary);
   //! [ordinary]

   //! [embedded]
   auto embedded = createLatDef(SizeParam<LatType::EMBEDDED>(2, 5), {1, 7, 9});
   std::cout << "embedded lattice:" << std::endl;
   printLatDef(embedded);
   //! [embedded]

   return 0;
}
