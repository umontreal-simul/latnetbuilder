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

#ifndef NETBUILDER__TASK__RANDOM_CBC_EXPLORER_H
#define NETBUILDER__TASK__RANDOM_CBC_EXPLORER_H

#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"

#include "latbuilder/LFSR258.h"

#include <vector>
#include <algorithm>

namespace NetBuilder { namespace Task {

/**
 * Class to explorer randomly a search space using the CBC search algorithm. 
 */ 
template <NetConstruction NC, EmbeddingType ET>
class RandomCBCExplorer
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:

        /** Constructor.
         * @param dimension Number of coordinates of the explorer.
         * @param sizeParameter Size parameter of the search space.
         * @param nbTries Number of random choices of generating values by dimension.
         */
        RandomCBCExplorer(Dimension dimension, typename ConstructionMethod::SizeParameter sizeParameter, unsigned int nbTries):
            m_dimension(dimension),
            m_currentCoord(0),
            m_nbTries(nbTries),
            m_randomGenValueGenerator(std::move(sizeParameter)),
            m_countTries(0)
        {};

        /**
         * Returns whether current coordinate is fully explored
         */ 
        bool isOver() 
        {
            return size() == count();
        }

        /**
         * Returns the next generating values of dimension \c dim
         */
        typename ConstructionMethod::GenValue nextGenValue()
        {
            m_countTries+= 1;
            return m_randomGenValueGenerator(m_currentCoord);
        }

        /**
         * Resets the explorer to the first coordinate.
         */ 
        void reset()
        {
           switchToCoordinate(0);
        }

        /**
         * Switches the explorer to coordinate \c coord.
         */ 
        void switchToCoordinate(Dimension coord)
        {
            m_currentCoord = coord;
            m_countTries = 0;
        };

        size_t size() const
        {
            if (NetConstructionTraits<NC>::hasSpecialFirstCoordinate && m_currentCoord == 0)
            {
                return 1;
            }
            return m_nbTries;
        }

        size_t count() const
        {
            return m_countTries;
        }

    private:
        Dimension m_dimension;
        Dimension m_currentCoord;
        unsigned int m_nbTries;
        typename ConstructionMethod:: template RandomGenValueGenerator <ET> m_randomGenValueGenerator;
        unsigned int m_countTries;
};

}}

#endif
