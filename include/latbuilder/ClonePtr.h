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
