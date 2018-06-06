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

#include "latbuilder/Types.h"

#include <iostream>
#include <limits>

using namespace LatBuilder;

int main(int argc, const char *argv[])
{
    Polynomial modulus;
    std::cin >> modulus;
    Polynomial m_stride(1);
    
    std::cout << modulus <<std::endl;
    Polynomial p = LatticeTraits<LatticeType::POLYNOMIAL>::ToGenValue(4);
    std::cout << p <<std::endl;
    auto i = LatticeTraits<LatticeType::POLYNOMIAL>::ToKernelIndex(LatticeTraits<LatticeType::POLYNOMIAL>::ToIndex(m_stride * p % modulus), modulus);
    std::cout << i <<std::endl;
    return 0;
}
