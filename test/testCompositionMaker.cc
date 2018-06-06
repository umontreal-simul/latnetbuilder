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

#include <vector>
#include <cstdlib>
#include <iostream>
#include <chrono>

#include "netbuilder/CompositionMaker.h"

void output(const std::vector<unsigned int> composition)
{
    for(const auto & x : composition)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

int main()
{

    unsigned int n = 15;
    unsigned int nb_parts = 10 ;

    CompositionMaker compMaker(n, nb_parts);

    output(compMaker.currentComposition());

    int count = 1;
    while( compMaker.goToNextComposition())
    {
        count++;
        auto change = compMaker.changeFromPreviousComposition();
        std::cout << change.first.first << ", " << change.first.second << " / " << change.second.first << ", " << change.second.second << std::endl;
        output(compMaker.currentComposition());
    }

}