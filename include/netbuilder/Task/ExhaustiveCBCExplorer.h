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

#ifndef NETBUILDER__TASK__EXHAUSTIVE_CBC_EXPLORER_H
#define NETBUILDER__TASK__EXHAUSTIVE_CBC_EXPLORER_H

#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"

#include <memory>

namespace NetBuilder { namespace Task {

template <NetConstruction NC>
class ExhaustiveCBCExplorer
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:


        ExhaustiveCBCExplorer(unsigned int dimension):
            m_dimension(dimension),
            m_currentDim(1),
            m_data(ConstructionMethod::genValueSpace(1)),
            m_state(0)
        {};

        bool isOver(unsigned int dim) 
        {
            if (m_state==m_data.size() && m_currentDim<=m_dimension)
            {
                if (m_currentDim < m_dimension)
                {
                    ++m_currentDim;
                    m_data = ConstructionMethod::genValueSpace(m_currentDim);
                    m_state = 0;
                }
                return true;
            }
            return false;
        }

        typename ConstructionMethod::GenValue nextGenValue(unsigned int dim)
        {
            m_state+=1;
            return m_data[m_state-1];
        }

    private:
        unsigned int m_dimension;
        std::vector<typename ConstructionMethod::GenValue> m_data;
        unsigned int m_currentDim;
        size_t m_state;
};

}}

#endif
