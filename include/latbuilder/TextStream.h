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

#ifndef LATBUILDER__TEXT_STREAM_H
#define LATBUILDER__TEXT_STREAM_H

#include <type_traits>
#include <ostream>
#include <sstream>
#include "latbuilder/detail/TextStream.h"

namespace LatBuilder {

/**
 * Overloaded stream operators.
 *
 * Provides stream operators for all classes that define a \c const_iterator
 * type, such as STL containers.
 *
 * To bring the operators defined in TextStream in the current scope, simply
 * use:
 * \code
 * using TextStream::operator<<;
 * \endcode
 *
 * If the preprocessor macro TEXTSTREAM_DISPLAY_COUNT is defined, the number of
 * elements printed to the stream is appended to the output.  For example,
 * without defining that macro, the following code:
 * \code
 * using TextStream::operator<<;
 * std::cout << std::vector<int>{1, 2, 3, 4, 5} << std::endl;
 * \endcode
 * outputs:
 * \code
 * [1, 2, 3, 4, 5]
 * \endcode
 * whereas it ouputs:
 * \code
 * [1, 2, 3, 4, 5]/5
 * \endcode
 * when TEXTSTREAM_DISPLAY_COUNT is defined (before including TextStream.h).
 */
namespace TextStream {

//========================================================================
// Declarations
//========================================================================

/**
 * Overload of the output stream operator for sequences.
 *
 * This operator can be applied an instance of a class that defines a \c
 * const_iterator type, assuming that the members \c begin() and \c end() are
 * also defined.  If the class defines a \c key_type type, curly brackets are
 * used as delimitors; otherwise, square brackets are used.
 */
template <typename T>
typename std::enable_if<detail::has_const_iterator<T>::value and !detail::is_ostreamable<T>::value, std::ostream&>::type
operator<<(std::ostream& os, const T& x);

/**
 * Overload of the output stream operator for pair.
 */
template <typename T>
typename std::enable_if<detail::is_pair<T>::value, std::ostream&>::type
operator<<(std::ostream& os, const T& x);


//========================================================================
// Implementation
//========================================================================

// for sequences
template <typename T>
typename std::enable_if<detail::has_const_iterator<T>::value and !detail::is_ostreamable<T>::value, std::ostream&>::type
operator<<(std::ostream& os, const T& x)
{
   unsigned count = 0;
   using std::operator<<;
   using TextStream::operator<<;
   os << detail::bracket_traits<T>::opening;
   for (const auto& xi : x) {
      std::ostringstream temp;
      temp << xi;
      std::string str = temp.str();
      if (str.back() == '\n'){
        if (count > 0)
        {
            os << ",\n " << str.substr(0, str.size()-1);
        }
        else
        {
            os << str.substr(0, str.size()-1);
        }
      }
      else{
        if (count > 0)
        {
            os << ", " << str;
        }
        else
        {
            os << str;
        }
      }
      count++;
   }
   os << detail::bracket_traits<T>::closing;
#ifdef TEXTSTREAM_DISPLAY_COUNT
   os << "/" << count;
#endif
   return os;
}

// for pairs
template <typename T>
typename std::enable_if<detail::is_pair<T>::value, std::ostream&>::type
operator<<(std::ostream& os, const T& x)
{
   using std::operator<<;
   using TextStream::operator<<;
   os << x.first;
   os << ":";
   os << x.second;
   return os;
}

}}

#endif
