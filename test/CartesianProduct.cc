// // This file is part of LatNet Builder.
// //
// // Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.

#include <iostream>
#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"

using namespace NetBuilder;

int main(int argc, char** argv)
{
    typedef NetConstructionTraits<NetConstruction::SOBOL> ConstructionMethod;

    auto seq = ConstructionMethod::GenValueSpaceCoordSeq(0);
    for(const auto& genVal : seq)
    {
        std::cout << genVal.first << std::endl;
        for(const auto& dirNum : genVal.second)
        {
            std::cout << dirNum << " ";
        }
        std::cout << std::endl;
    }    
}