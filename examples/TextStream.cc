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

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "latbuilder/TextStream.h"

namespace CustomSpace {

   struct CustomType {
      CustomType(const std::string& s): value(s) {}
      std::string value;
   };

   std::ostream& operator<<(std::ostream& os, const CustomType& x)
   { return os << "CustomType(" << x.value << ")"; }

}

using namespace CustomSpace;
using namespace LatBuilder;
using namespace std;

int main(int argc, const char *argv[])
{
   using TextStream::operator<<;
   std::cout << "TextStream Test" << std::endl;
   // output the content of STL containers
   std::cout << vector<int>{1, 2, 3, 4, 5} << std::endl;
   std::cout << list<string>{"abc", "def", "ghi", "xyz"} << std::endl;
   std::cout << pair<string, int>{"abc", 3} << std::endl;
   std::cout << map<string, int>{{"abc", 3}, {"def", 7}, {"ghi", 10}} << std::endl;
   // output the content of an STL container recursively
   std::cout << map<string, vector<int>>{{"abc", {1,2,3}}, {"def", {4,5,6}}, {"ghi", {7,8,9}}} << std::endl;
   // output custom object types from an STL container
   std::cout << list<CustomType>{CustomType("abc"), CustomType("def")} << std::endl;
   // test delegation to std
   std::cout << std::setw(12);
   return 0;
}
