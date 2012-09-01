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

#ifndef LATCOMMON__COORDINATES_H
#define LATCOMMON__COORDINATES_H

#include <iterator>
#include <set>
#include <map>
#include <iostream>
#include "latcommon/UtilLC.h"


namespace LatCommon {

/**
 * Set of coordinates.
 *
 * \remark Coordinates indices start at 0.  To make the input/output
 * representation of coordinates start at 1 instead of 0, see #humanize.  For
 * code written with coordinate indices starting at 1, set #humanize to \c false.
 */
class Coordinates : public std::set<size_t> {
public:
   /**
    * &ldquo;Humanize&rdquo; the formatting of coordinate values.
    *
    * If set to \c true, the external representation of coordinate values if
    * shifted by one with respect to its internal representation.  More
    * precisely, an internal coordinate value \f$j\f$ is mapped to the external
    * representation \f$j+1\f$ during output, and an external coordinate value
    * \f$j+1\f$ is mapped to internal representation \f$j\f$ during input.
    *
    * Defaults to \c true;
    *
    * \sa #asOutput() #asInput()
    */
   static bool humanize;

   /**
    * Maps the internal representation of a coordinate value to its external
    * representation.
    */
   inline static value_type asOutput(value_type i)
   { return i + (humanize ? 1 : 0); }

   /**
    * Maps the external representation of a coordinate value to its internal
    * representation.
    */
   inline static value_type asInput(value_type i)
   { return i - (humanize ? 1 : 0); }

   /**
    * Constructs an empty coordinate set.
    */
   Coordinates():
      std::set<value_type>()
   { }

   /**
    * Copy-constructor.
    */
   Coordinates(const Coordinates& other):
        std::set<value_type>(other)
   { }

   /**
    * Constructs a coordinate set populated with the values from \c first
    * (inclusively) to \c last (exclusively).
    */
   template<typename InputIterator>
     Coordinates(InputIterator first, InputIterator last):
        std::set<value_type>(first, last)
   { }
};


/**
 * \relates Coordinates
 * Formats the coordinate set \c coords and outputs it to \c os.
 */
std::ostream& operator<< (std::ostream& os, const Coordinates& coords);

/**
 * \relates Coordinates
 * Reads a formatted coordinate set from \c is.
 *
 * The input must consist of positive integers separated by whitespace and/or by
 * commas, and optionally enclosed in braces.  The ordering is not important.
 * Repeated values are ignored.
 * For example, the following strings are valid input that would produce
 * equivalent Coordinates objects:
 * - <tt>1 2 5</tt>
 * - <tt>1, 2, 5</tt>
 * - <tt>{1 2 5}</tt>
 * - <tt>{1,2,5}</tt>
 * - <tt>{1, 2, 5}</tt>
 * - <tt>2 5 1</tt>
 * - <tt>2 1 5 1</tt>
 */
std::istream& operator>> (std::istream& is, Coordinates& coords);

}
#endif
