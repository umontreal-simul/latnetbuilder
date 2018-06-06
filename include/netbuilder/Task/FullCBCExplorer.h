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

#ifndef NETBUILDER__TASK__FULL_CBC_EXPLORER_H
#define NETBUILDER__TASK__FULL_CBC_EXPLORER_H

#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"

#include <memory>

namespace NetBuilder { namespace Task {

/**
 * Class to explorer exhaustively a search space using the CBC search algorithm. 
 */ 
template <NetConstruction NC>
class FullCBCExplorer
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:


        /**
         * Constructor.
         * @param dimension Maximal dimension of the explorer.
         * @param designParameter Design parameter of the search space.
         * @param verbose Verbosity level.
         */ 
        FullCBCExplorer(unsigned int dimension, typename ConstructionMethod::DesignParameter designParameter, int verbose = 0):
            m_dimension(dimension),
            m_designParameter(std::move(designParameter)),
            m_currentDim(0),
            m_state(0),
            m_verbose(verbose)
        {};

        /**
         * Returns whether the dimension \c dim is fully explored
         */ 
        bool isOver(unsigned int dim) 
        {
            return m_state==m_data.size() && m_currentDim<=m_dimension;
        }

        /**
         * Returns the next generating values of dimension \c dim
         */ 
        typename ConstructionMethod::GenValue nextGenValue(unsigned int dim)
        {
            m_state+=1;
            if(this->m_verbose>0)
            {
                std::cout << "Dimension: " << dim << "/" << m_dimension <<  " - ";
                std::cout << "net " << m_state << "/" << m_data.size() << std::endl;
            }
            return m_data[m_state-1];
        }

        /**
         * Resets the explorer to the first dimension
         */ 
        void reset()
        {
            switchToDimension(1);
        }

        /**
         * Sets the verbosity level of the explorer.
         */ 
        void setVerbose(int verbose)
        {
            m_verbose = verbose;
        }

        /**
         * Switches the explorer to dimension \c dim.
         */ 
        void switchToDimension(unsigned int dim)
        {
            m_currentDim = dim;
            m_data = ConstructionMethod::genValueSpaceDim(m_currentDim,  m_designParameter);
            m_state = 0;
        }

    private:
        unsigned int m_dimension;
        typename ConstructionMethod::DesignParameter m_designParameter;
        unsigned int m_currentDim;
        std::vector<typename ConstructionMethod::GenValue> m_data;
        size_t m_state;
        int m_verbose;
};

template<> void FullCBCExplorer<NetConstruction::POLYNOMIAL>::switchToDimension(unsigned int dim)
{
    if(m_currentDim == 1 && dim != 1)
    {
        m_data = ConstructionMethod::genValueSpaceDim(dim,  m_designParameter);
    }
    if(dim == 1 && m_currentDim != 1)
    {
        m_data = ConstructionMethod::genValueSpaceDim(dim,  m_designParameter);
    }
    m_state = 0;
    m_currentDim = dim;
}


}}

#endif
