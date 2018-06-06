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

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include "latbuilder/IndexMap.h"
#include "latbuilder/TextStream.h"

class ReverseMapper {
public:
   typedef size_t size_type;
   ReverseMapper(size_type size): m_size(size) { }
   size_type size() const { return m_size; }
   size_type operator() (size_type i) const { return m_size - i - 1; }
private:
   size_type m_size;
};

template <typename VEC>
void set_elems(VEC& vec)
{
   for (unsigned i = 0; i < vec.size(); i++)
      vec[i] = 10 + i;
}

int main(int argc, char* argv[])
{
   using LatBuilder::TextStream::operator<<;

   boost::numeric::ublas::vector<unsigned> vec(10);
   auto rvec = LatBuilder::permuteVector(vec, ReverseMapper(vec.size()));

   std::cout << "==> setting original vector" << std::endl;
   set_elems(vec);
   std::cout << " orginal vector: " << vec << std::endl;
   std::cout << "reversed vector: " << rvec << std::endl;

   std::cout << "==> setting reversed vector" << std::endl;
   set_elems(rvec);
   std::cout << " orginal vector: " << vec << std::endl;
   std::cout << "reversed vector: " << rvec << std::endl;

   return 0;
}

