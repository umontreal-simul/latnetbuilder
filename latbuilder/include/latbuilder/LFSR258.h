// Copyright (c) 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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

#ifndef LATBUILDER__LFSR258_H
#define LATBUILDER__LFSR258_H

#include <limits>
#include <array>
#include <stdexcept>
#include <cstdint>

namespace LatBuilder {

/**
 * LFSR258 pseudo-random generator by L'Ecuyer.
 * This class generates 64-bit pseudo-random numbers using the <tt>LFSR258</tt>
 * generator by L'Ecuyer \cite rLEC99a, with a period length near \f$2^{258}\f$.
 */
class LFSR258 {
public:
   typedef uint64_t result_type;
   typedef std::array<result_type, 5> seed_type;

   static const seed_type default_seed;

   /**
    * Constructor.
    *
    * \tparam s   Optional seed.
    */
   LFSR258(seed_type s = default_seed) { seed(std::move(s)); }

   /**
    * Returns the current seed.
    */
   const seed_type& seed()
   { return m_s; }

   /**
    * Sets the seed for the generator.
    */
   void seed(seed_type s) {
      check_seed(s);
      m_s = std::move(s);
   }

   /**
    * Jumps 2^100 iterations past the current state.
    */
   void jump();

   /**
    * Returns the smallest value in the output range.
    */

   result_type min() const
   { return 0; }

   /**
    * Returns the largest value in the output range.
    */
   result_type max() const
   { return std::numeric_limits<result_type>::max(); }

   /**
    * Returns the next random number in the sequence.
    */
   result_type operator()();

   /**
    * Returns \c true if the generators \c e1 and \c e2 are in the same state,
    * or else \c false.
    */
   friend bool operator==(const LFSR258& e1, const LFSR258& e2)
   { return e1.m_s == e2.m_s; }

   /**
    * Returns \c true if the generators \c e1 and \c e2 are not in the same
    * state, or else \c false.
    */
   friend bool operator!=(const LFSR258& e1, const LFSR258& e2)
   { return not (e1 == e2); }

private:
   seed_type m_s;
   void check_seed(const seed_type& s);
};

}
#endif
