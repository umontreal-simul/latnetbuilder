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

#include <iostream>
#include <vector>
#include "latbuilder/TextStream.h"

#include "Path.h"

using namespace LatBuilder;

int main()
{
   SET_PATH_TO_LATNETBUILDER_FOR_EXAMPLES();
   //! [cout]
   using TextStream::operator<<;
   std::cout << std::vector<int>{1, 2, 3, 4, 5} << std::endl;
   //! [cout]
   return 0;
}
