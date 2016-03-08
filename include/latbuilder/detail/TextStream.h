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

#ifndef LATBUILDER__DETAIL__TEXT_STREAM_H
#define LATBUILDER__DETAIL__TEXT_STREAM_H

#include <type_traits>

namespace LatBuilder { namespace TextStream { namespace detail {

//========================================================================
// Type traits
//========================================================================

template <typename T>
class is_ostreamable {
private:
   template <typename> static std::false_type helper(...);
   template <typename Tp> static std::true_type helper(typename std::remove_reference<decltype(*((std::ostream*)nullptr) << *((Tp*)nullptr))>::type*);
   typedef decltype(helper<T>(nullptr)) value_type;
public:
   constexpr static bool value = value_type::value;
};

template <typename T> class has_const_iterator {
private:
   template <typename> static std::false_type helper(...);
   template <typename Tp> static std::true_type helper(typename Tp::const_iterator*);
   typedef decltype(helper<T>(nullptr)) value_type;
public:
   constexpr static bool value = value_type::value;
};

template <typename T> class is_pair {
private:
   template <typename> static std::false_type helper(...);
   template <typename Tp> static std::true_type helper(typename Tp::first_type*, typename Tp::second_type*);
   typedef decltype(helper<T>(nullptr, nullptr)) value_type;
public:
   constexpr static bool value = value_type::value;
};

template <typename T> class is_set {
private:
   template <typename> static std::false_type helper(...);
   template <typename Tp> static std::true_type helper(typename Tp::key_type*);
   typedef decltype(helper<T>(nullptr)) value_type;
public:
   constexpr static bool value = value_type::value;
};

template <bool> struct bracket_traits_helper {
   static constexpr char opening = '[';
   static constexpr char closing = ']';
};

template <> struct bracket_traits_helper<true> {
   static constexpr char opening = '{';
   static constexpr char closing = '}';
};

template <typename T> struct bracket_traits : public detail::bracket_traits_helper<is_set<T>::value> {};

}}}

#endif
