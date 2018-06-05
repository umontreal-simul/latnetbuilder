// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
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

#ifndef NETBUILDER__CBC_COORDINATE_SET_H
#define NETBUILDER__CBC_COORDINATE_SET_H

#include "latticetester/CoordinateSets.h"

#include <iostream>
#include <algorithm>

namespace NetBuilder { 

/** 
 * CBC sequence of coordinate sets.
 * 
 * This class implements a sequence of coordinates (@see LatticeTester::CoordinateSets) which can be used in the CBC
 * evaluation of figures of merit.
 */ 
class CBCCoordinateSet
{
    public:

        /** Constructs the set of all the non empty subsets of \f$\{0,\dots, \c maxCoordinate\}\f$ with order lower than \c maxOrder
         *  containing \c maxCoordinate.
         * @param maxCoordinate Maximal coordinate of the subsets.
         * @param maxOrder Maximal order of subsets.
         */ 
        CBCCoordinateSet(int maxCoordinate, int maxOrder):
            m_dimension(maxCoordinate),
            m_maxOrder(maxOrder),
            m_base(0, std::min(maxOrder-1,maxCoordinate-1), 0, std::max(maxCoordinate-2,0)),
            m_addCoordinate(m_base, maxCoordinate-1)
        {};

        /**
        * Returns an iterator pointing to the first element in the sequence.
        */
        LatticeTester::CoordinateSets::AddCoordinate<LatticeTester::CoordinateSets::FromRanges>::const_iterator begin() const 
        {
            return m_addCoordinate.begin();
        }

        /**
        * Returns an iterator pointing past the last element in the sequence.
        */
        LatticeTester::CoordinateSets::AddCoordinate<LatticeTester::CoordinateSets::FromRanges>::const_iterator end() const 
        {
            return m_addCoordinate.end();
        }

    private:
        unsigned int m_dimension;
        unsigned int m_maxOrder;
        LatticeTester::CoordinateSets::FromRanges m_base;
        LatticeTester::CoordinateSets::AddCoordinate<LatticeTester::CoordinateSets::FromRanges> m_addCoordinate;
};

}

#endif