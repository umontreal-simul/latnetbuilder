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

#include "netbuilder/SobolDirectionNumbers.h"

#include "latbuilder/TextStream.h"
#include "latbuilder/SeqCombiner.h"
#include "latbuilder/LFSR258.h"

#include <iostream>

using namespace NetBuilder;
using LatBuilder::TextStream::operator<<;


int main(int argc, const char *argv[])
{

    auto s1 = SobolDirectionNumbers<>(1);
    auto s2 = SobolDirectionNumbers<>(2);
    auto s3 = SobolDirectionNumbers<>(3);
    auto s4 = SobolDirectionNumbers<>(4);

    std::vector<SobolDirectionNumbers<>> s;
    s.push_back(s1);
    s.push_back(s2);
    s.push_back(s3);
    s.push_back(s4);

    LatBuilder::SeqCombiner<SobolDirectionNumbers<>,LatBuilder::CartesianProduct> foo(s);
    //std::cout << foo << std::endl;

    std::vector<std::vector<unsigned long>> toto;
    
    toto.assign(foo.begin(),foo.end());

    std::cout << toto << std::endl;

    /*typedef LatBuilder::Traversal::Random<LatBuilder::LFSR258> Trav;

    Trav trav(10); 

    auto s1 = SobolDirectionNumbers<Trav>(1,trav);
    //std::cout << s1 << std::endl;

    std::cout << std::endl;

    auto s2 = SobolDirectionNumbers<Trav>(2,trav);
    std::cout << s2 << std::endl;

    auto s3 = SobolDirectionNumbers<Trav>(3,trav);
    std::cout << s3 << std::endl;

    auto s4 = SobolDirectionNumbers<Trav>(4,trav);
    std::cout << s4 << std::endl;

    //std::vector<int> s1 = {1,2,3,4};
    //std::vector<int> s2 = {5,6,7,1};

    //std::vector<std::vector<int>> s;
    std::vector<SobolDirectionNumbers<>> s;
    s.push_back(s1);
    s.push_back(s2);
    s.push_back(s3);
    s.push_back(s4);

    LatBuilder::SeqCombiner<SobolDirectionNumbers<>,LatBuilder::Zip> foo(s);
   

    //LatBuilder::SeqCombiner<std::vector<int>, LatBuilder::Zip> foo(s);

    std::cout << foo << std::endl;*/

}
