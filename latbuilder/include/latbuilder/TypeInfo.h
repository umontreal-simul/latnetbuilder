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
   class max;
   class sum;
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
