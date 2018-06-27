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

#ifndef NETBUILDER__TASK__FULL_CBC_EXPLORER_H
#define NETBUILDER__TASK__FULL_CBC_EXPLORER_H

#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"

#include <memory>

namespace NetBuilder { namespace Task {

/**
 * Class to explorer exhaustively a search space using the CBC search algorithm. 
 */ 
template <NetConstruction NC, EmbeddingType ET>
class FullCBCExplorer
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:

        /**
         * Constructor.
         * @param dimension Number of coordinates of the explorer.
         * @param designParameter Design parameter of the search space.
         */ 
        FullCBCExplorer(Dimension dimension, typename ConstructionMethod::DesignParameter designParameter):
            m_dimension(dimension),
            m_currentCoord(0),
            m_designParameter(std::move(designParameter)),
            m_data(ConstructionMethod::genValueSpaceCoord(m_currentCoord,  m_designParameter)),
            m_state(m_data.begin()),
            m_count(0)
        {};

        /**
         * Returns whether the current coordinate is fully explored
         */ 
        bool isOver() 
        {
            return m_state==m_data.end();
        }

        /**
         * Returns the next generating value for the current coordinate.
         */ 
        typename ConstructionMethod::GenValue nextGenValue()
        {
            auto val = *m_state;
            m_state++;
            m_count++;
            return val;
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
            m_count = 0;
            m_data = ConstructionMethod::genValueSpaceCoord(m_currentCoord,  m_designParameter);
            m_state = m_data.begin();
        }

        size_t size() const
        {
            return m_data.size();
        }

        size_t count() const
        {
            return m_count;
        }

    private:
        Dimension m_dimension;
        Dimension m_currentCoord;
        typename ConstructionMethod::DesignParameter m_designParameter;
        typename ConstructionMethod::GenValueSpaceCoordSeq m_data;
        typename ConstructionMethod::GenValueSpaceCoordSeq::const_iterator m_state;
        size_t m_count;
        int m_verbose;
};


}}

#endif
