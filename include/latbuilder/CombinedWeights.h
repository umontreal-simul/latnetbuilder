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

#ifndef LATBUILDER__COMBINED_WEIGHTS_H
#define LATBUILDER__COMBINED_WEIGHTS_H

#include "latticetester/Weights.h"
#include <list>
#include <memory>


namespace LatBuilder {
/**
 * Combined weights.
 *
 * Sum of different instances of weights (possibly be of different concrete
 * types).
 */
class CombinedWeights : public LatticeTester::Weights {
public:

   typedef std::list<std::unique_ptr<LatticeTester::Weights>> WeightsList;

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
   virtual LatticeTester::Weight getWeight (const LatticeTester::Coordinates & projection) const;

//    virtual std::string name() const { return "combined"; }

   /**
    * Adds a weight specification to the set of combined weights.
    */
   void add (std::unique_ptr<LatticeTester::Weights> weights) 
   { m_weights.push_back(std::move(weights)); }

   const WeightsList& list() const
   { return m_weights; }

   WeightsList giveWeights(){
       return std::move(m_weights);
   }

    /**
     * {@inheritDoc}
     */ 
   virtual unsigned int interlacingFactor() const{
       if (m_weights.size() == 0)
       {
           return 1;
       }
       else
       {
           return m_weights.front()->interlacingFactor();
       }
   }

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
 *   #operator>>(std::istream&, LatticeTester::Coordinates&)
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
 * \sa  #operator>>(std::istream&, LatticeTester::Coordinates&)
 */
std::istream& operator>> (std::istream& is, CombinedWeights& weights);
#endif
}
#endif
