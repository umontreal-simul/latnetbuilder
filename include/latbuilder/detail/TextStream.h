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

#ifndef LATBUILDER__DETAIL__TEXT_STREAM_H
#define LATBUILDER__DETAIL__TEXT_STREAM_H

#include <type_traits>
#include <iostream>

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
