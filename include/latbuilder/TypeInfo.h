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

#ifndef LATBUILDER__TYPE_INFO_H
#define LATBUILDER__TYPE_INFO_H

#include <typeinfo>
#include <string>

namespace LatBuilder {

/**
 * Framework to specify a human-readable name for data types.
 */
template <typename T> struct TypeInfo {

   /// By default use name() from compiler type info
   template <typename> static const char* guessName(...)
   { return typeid(T).name(); }

   /// If the class provides a name() function, use it.
   template <typename Tp> static decltype(Tp::name()) guessName(const Tp*)
   { return Tp::name(); }

   /// Returns the name of type \c T.
   static std::string name() { return guessName<T>(nullptr); }
};

}


// specializations

namespace boost { namespace accumulators { namespace tag {
   struct max;
   struct sum;
}}}

namespace LatBuilder {

template <> struct TypeInfo<boost::accumulators::tag::max> {
   constexpr static const char* name() { return "max"; }
};
template <> struct TypeInfo<boost::accumulators::tag::sum> {
   constexpr static const char* name() { return "sum"; }
};

#define DECLARE_TYPEINFO(T) \
   template <> struct TypeInfo<T> { \
      constexpr static const char* name() { return #T; } \
   }

DECLARE_TYPEINFO(float);
DECLARE_TYPEINFO(double);
DECLARE_TYPEINFO(int);
DECLARE_TYPEINFO(unsigned int);
DECLARE_TYPEINFO(long);
DECLARE_TYPEINFO(unsigned long);
DECLARE_TYPEINFO(long long);
DECLARE_TYPEINFO(unsigned long long);

#undef DECLARE_TYPEINFO

}

#endif
