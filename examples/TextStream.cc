// Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
// 
// This file is part of Lattice Builder.
// 
// Lattice Builder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Lattice Builder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

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
