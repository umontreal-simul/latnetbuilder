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

#ifndef NETBUILDER__TASK__MIXED_CBC_EXPLORER_H
#define NETBUILDER__TASK__MIXED_CBC_EXPLORER_H

#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/Task/FullCBCExplorer.h"
#include "netbuilder/Task/RandomCBCExplorer.h"

namespace NetBuilder { namespace Task {

/**
 * Class to explorer first exhaustively and then randomly a search space using the CBC search algorithm. 
 */ 
template <NetConstruction NC, EmbeddingType ET>
class MixedCBCExplorer
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:

        /** Constructor.
         * @param dimension Number of coordinates of the explorer.
         * @param sizeParameter Size parameter of the search space.
         * @param nbFullCoordinates Number of coordinates for which the search is exhaustive.
         * @param nbTries Number of random choices of generating values.
         */
        MixedCBCExplorer(Dimension dimension, typename ConstructionMethod::SizeParameter sizeParameter, unsigned int nbFullCoordinates, unsigned int nbTries):
            m_dimension(dimension),
            m_currentCoord(0),
            m_nbFullCoordinates(nbFullCoordinates),
            m_randExplorer(new RandomCBCExplorer<NC, ET>(dimension, sizeParameter, nbTries)),
            m_fullExplorer(new FullCBCExplorer<NC, ET>(nbFullCoordinates, sizeParameter))
        {};

        /**
         * Returns whether the dimension \c dim is fully explored
         */ 
        bool isOver()
        {
            if (m_currentCoord < m_nbFullCoordinates)
            {
                return m_fullExplorer->isOver();
            }
            else
            {
                return m_randExplorer->isOver();
            }
        }

        /**
         * Returns the next generating values of dimension \c dim
         */ 
        typename ConstructionMethod::GenValue nextGenValue()
        {
            if (m_currentCoord < m_nbFullCoordinates)
            {
                return m_fullExplorer->nextGenValue();
            }
            else
            {
                return m_randExplorer->nextGenValue();
            } 
        }

        /**
         * Resets the explorer to the first coordinate.
         */ 
        void reset() 
        {
            m_currentCoord = 0;
            m_randExplorer.reset();
            m_fullExplorer.reset();
        }

        /**
         * Switches the explorer to coordinate \c coord.
         */ 
        void switchToCoordinate(Dimension coord)
        {
            m_currentCoord = coord;
            if (coord < m_nbFullCoordinates)
            {
                return m_fullExplorer->switchToCoordinate(coord);
            }
            else
            {
                return m_randExplorer->switchToCoordinate(coord);
            } 
        }; 

        size_t size() const
        {
            if (m_currentCoord < m_nbFullCoordinates)
            {
                return m_fullExplorer->size();
            }
            else
            {
                return m_randExplorer->size();
            } 
        }

        size_t count() const
        {
            if (m_currentCoord < m_nbFullCoordinates)
            {
                return m_fullExplorer->count();
            }
            else
            {
                return m_randExplorer->count();
            } 
        }

    private:
        Dimension m_dimension;
        Dimension m_currentCoord;
        unsigned int m_nbFullCoordinates;
        std::unique_ptr<RandomCBCExplorer<NC, ET>> m_randExplorer;
        std::unique_ptr<FullCBCExplorer<NC, ET>> m_fullExplorer;
};

}}

#endif
