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

   static constexpr result_type min()
   { return 0; }

   /**
    * Returns the largest value in the output range.
    */
   static constexpr result_type max()
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
