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

#ifndef LATCOMMON__PROJECTION_DEPENDENT_WEIGHTS_H
#define LATCOMMON__PROJECTION_DEPENDENT_WEIGHTS_H

#include "latcommon/Weights.h"
#include <map>
#include <vector>
#ifdef WITH_XML
#include <pugixml.hpp>
#endif


namespace LatCommon {
/**
 * Projection-dependent weights.
 *
 * The weight for a given projection can be set with #setWeight().
 *
 * Internally, the weights are regrouped by largest coordinate index in
 * different std::map objects.  This is useful for use with CBC.
 */
class ProjectionDependentWeights : public Weights {
protected:

   typedef std::map<Coordinates, Weight> WeightsMap;

   /// Per-projection weights, regrouped by largest coordinate index.
   std::vector<WeightsMap> m_weights;

   /// Used only to return an empty map.
   static const WeightsMap m_emptyWeights;

public:

   /**
    * Constructs projection-dependent weights.
    *
    * \param defaultWeight    Default weights.
    */
   ProjectionDependentWeights();

   /**
    * Destructor.
    */
   virtual ~ProjectionDependentWeights()  {} 

   /**
    * Copy constructor.
    *
    * \todo S'il y a un constructeur de recopie, il devrait prob. y avoir une
    * méthode \c operator=
    */
   ProjectionDependentWeights (const ProjectionDependentWeights &);

   /**
    * Returns the weight of the projection specified by \c projection.
    */
   virtual Weight getWeight (const Coordinates & projection) const;

   /**
    * Returns a map of weights for all projections whose largest index is \c
    * largestIndex.
    */
   const WeightsMap& getWeightsForLargestIndex(Coordinates::value_type largestIndex) const;

#ifdef WITH_XML
   /**
    * Static factory method; create a ProjectionDependentWeights object by
    * parsing XML data.
    */
   static ProjectionDependentWeights* createFromXML (const pugi::xml_node& node);
#endif

   /**
    * Sets the weight of the projection specified by \c projection.
    */
   void setWeight (const Coordinates & projection, Weight weight);

protected:
   /// \copydoc LatCommon::Weights::format()
   virtual void format(std::ostream& os) const;

   friend std::istream& operator>> (std::istream&, ProjectionDependentWeights&);
};

/**
 * \relates ProjectionDependentWeights
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
std::istream& operator>> (std::istream& is, ProjectionDependentWeights& weights);

}
#endif
