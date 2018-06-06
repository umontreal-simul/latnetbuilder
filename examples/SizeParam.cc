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

#include "latbuilder/SizeParam.h"
#include <iostream>

using namespace LatBuilder;

template <LatType L>
void test(const SizeParam<L>& sizeParam)
{
   std::cout << "size parameter: " << sizeParam << std::endl;
   std::cout << "  points:       " << sizeParam.numPoints() << std::endl;
   std::cout << "  totient:      " << sizeParam.totient() << std::endl;
}

int main(int argc, const char *argv[])
{
   test(SizeParam<LatType::ORDINARY>(100));
   test(SizeParam<LatType::EMBEDDED>(256));
   return 0;
}
