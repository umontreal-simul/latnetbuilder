// Copyright (c) 2012, 2014 David Munger, Pierre L'Ecuyer, Université de Montréal.
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

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include "latbuilder/IndexMap.h"
#include "latbuilder/TextStream.h"

class ReverseMapper {
public:
   typedef unsigned size_type;
   ReverseMapper(size_type size): m_size(size) { }
   size_type size() const { return m_size; }
   size_type operator() (size_type i) const { return m_size - i - 1; }
private:
   unsigned m_size;
};

template <typename VEC>
void set_elems(VEC& vec)
{
   for (typename VEC::size_type i = 0; i < vec.size(); i++)
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

