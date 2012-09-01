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

#ifndef LATBUILDER__COMBINED_WEIGHTS_H
#define LATBUILDER__COMBINED_WEIGHTS_H

#include "latcommon/Weights.h"
#include <list>
#include <memory>


namespace LatBuilder {
/**
 * Combined weights.
 *
 * Sum of different instances of weights (possibly be of different concrete
 * types).
 */
class CombinedWeights : public LatCommon::Weights {
public:

   typedef std::list<std::unique_ptr<LatCommon::Weights>> WeightsList;

   /**
    * Constructs an empty set of combined weights.
    */
   CombinedWeights();

   /**
    * Destructor.
    */
   virtual ~CombinedWeights()  {} 

   /**
    * Returns the weight of the projection specified by \c projection.
    */
   virtual LatCommon::Weight getWeight (const LatCommon::Coordinates & projection) const;

   /**
    * Adds a weight specification to the set of combined weights.
    */
   void add (std::unique_ptr<LatCommon::Weights> weights) 
   { m_weights.push_back(std::move(weights)); }

   const WeightsList& list() const
   { return m_weights; }

protected:
   virtual void format(std::ostream& os) const;

#if 0
   friend std::ostream& operator<< (std::ostream&, const CombinedWeights&);
   friend std::istream& operator>> (std::istream&, CombinedWeights&);
#endif
protected:
   WeightsList m_weights;
};

#if 0
/**
 * \relates CombinedWeights
 * Reads formatted projection-dependent weights into the object \c weights.
 *
 * The input should be a sequence of projection-to-weight mappings, of the format:
 * \code
 * <match1>: <weight1>, <match2>: <weight2>, ...
 * \endcode
 * where <tt>\<weight<i>n</i>\></tt> is the weight (a floating point number)
 * associated to the projection-match <tt>\<match<i>n</i>\></tt>, and
 * <tt>\<match<i>n</i>\></tt> is one of:
 * - a set of coordinates, as specified in
 *   #operator>>(std::istream&, LatCommon::Coordinates&)
 *   to explicitly set the weight for the projection that
 *   correspond to these coordinates;
 * - the string <tt>order <i>m</i></tt> to implicitly set the weights of
 *   projections of order <tt><i>m</i></tt>;
 * - the string \c default to set the default weight for other projections.
 *
 * \remark Comments can be appended to any line after a \c # character.
 *
 * \remark The match-weight pairs can be separated by commas and/or whitespace
 * (including newlines).
 *
 * \remark The colons (\c :) can be replaced with <tt>=\></tt> or <tt>-\></tt>.
 *
 * \sa  #operator>>(std::istream&, LatCommon::Coordinates&)
 */
std::istream& operator>> (std::istream& is, CombinedWeights& weights);
#endif

/**
 * \relates CombinedWeights
 * Outputs the projection-dependent weights to \c os in a format readable.
 */
std::ostream& operator<< (std::ostream& os, const CombinedWeights& weights);

}
#endif
