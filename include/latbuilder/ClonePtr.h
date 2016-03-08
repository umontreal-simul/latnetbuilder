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

#ifndef LATBUILDER__CLONE_PTR_H
#define LATBUILDER__CLONE_PTR_H

#include <memory>

namespace LatBuilder {

/**
 * Copyable alternative to std::unique_ptr<> for cloneable objects.
 *
 * \tparam T Type of object pointed to.  Must implement a clone() member.
 */
template <typename T>
class ClonePtr : public std::unique_ptr<T> {
public:
   ClonePtr(T* t = nullptr):
      std::unique_ptr<T>(t)
   {}

   ClonePtr(const ClonePtr<T>& p):
      std::unique_ptr<T>(cloneAndCast(p))
   {}

   ClonePtr(const std::unique_ptr<T>& p):
      std::unique_ptr<T>(cloneAndCast(p))
   {}

   ClonePtr& operator=(const ClonePtr<T>& p)
   { return *this = ClonePtr(p->clone()); }

   ClonePtr& operator=(ClonePtr<T>&& p)
   { std::unique_ptr<T>::operator=(std::move(p)); return *this; }

private:
   static std::unique_ptr<T> cloneAndCast(const std::unique_ptr<T>& p)
   {
      return std::unique_ptr<T>(
            p ?
            dynamic_cast<T*>(p->clone().release()) :
            nullptr
            );
   }
};

}

#endif
