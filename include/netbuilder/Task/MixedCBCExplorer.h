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

#ifndef NETBUILDER__TASK__MIXED_CBC_EXPLORER_H
#define NETBUILDER__TASK__MIXED_CBC_EXPLORER_H

#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/Task/ExhaustiveCBCExplorer.h"
#include "netbuilder/Task/RandomCBCExplorer.h"

namespace NetBuilder { namespace Task {

template<NetConstruction NC>
class MixedCBCExplorer
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:

        MixedCBCExplorer(unsigned int dimension, unsigned int maxExhaustiveDimension, unsigned int nbTries):
            m_dimension(dimension),
            m_maxExhaustiveDimension(maxExhaustiveDimension),
            m_randExplorer(new RandomCBCExplorer<NC>(dimension, nbTries)),
            m_exhaustiveExplorer(new ExhaustiveCBCExplorer<NC>(dimension))
        {};

        ~MixedCBCExplorer() = default;

        bool isOver(unsigned int dim)
        {
            if (dim <= m_maxExhaustiveDimension)
            {
                return m_exhaustiveExplorer->isOver(dim);
            }
            else
            {
                return m_randExplorer->isOver(dim);
            }
        }

        typename ConstructionMethod::GenValue nextGenValue(unsigned int dim)
        {
            if (dim <= m_maxExhaustiveDimension)
            {
                return m_exhaustiveExplorer->nextGenValue(dim);
            }
            else
            {
                return m_randExplorer->nextGenValue(dim);
            } 
        }

    private:
        unsigned int m_dimension;
        unsigned int m_maxExhaustiveDimension;
        std::unique_ptr<RandomCBCExplorer<NC>> m_randExplorer;
        std::unique_ptr<ExhaustiveCBCExplorer<NC>> m_exhaustiveExplorer;
};

}}

#endif
