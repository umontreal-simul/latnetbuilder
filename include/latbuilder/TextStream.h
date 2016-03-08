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

#ifndef LATBUILDER__TEXT_STREAM_H
#define LATBUILDER__TEXT_STREAM_H

#include <type_traits>
#include <ostream>
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
      if (count > 0)
         os << ", ";
      os << xi;
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

/** \example TextStream.cc
 * This is an example of how to use the TextStream namespace.
 * It can be built by launching
 * \code
 * b2 TextStream
 * \endcode
 * from the \c examples/ directory.
 */

#endif
