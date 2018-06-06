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
#include "netbuilder/Task/FullCBCExplorer.h"
#include "netbuilder/Task/RandomCBCExplorer.h"

namespace NetBuilder { namespace Task {

/**
 * Class to explorer first exhaustively and then randomly a search space using the CBC search algorithm. 
 */ 
template<NetConstruction NC>
class MixedCBCExplorer
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:

        /** Constructor.
         * @param dimension Maximal dimension of the explorer.
         * @param designParameter Design parameter of the search space.
         * @param maxFullDimension Maximum dimension for which the search is exhaustive.
         * @param nbTries Number of random choices of generating values by dimension after dimension \c maxFullDimension.
         * @param verbose Verbosity level.
         */
        MixedCBCExplorer(unsigned int dimension, typename ConstructionMethod::DesignParameter designParameter, unsigned int maxFullDimension, unsigned int nbTries, int verbose = 0):
            m_dimension(dimension),
            m_maxFullDimension(maxFullDimension),
            m_randExplorer(new RandomCBCExplorer<NC>(dimension,designParameter, nbTries, verbose)),
            m_fullExplorer(new FullCBCExplorer<NC>(dimension,designParameter, verbose))
        {};

        /**
         * Returns whether the dimension \c dim is fully explored
         */ 
        bool isOver(unsigned int dim)
        {
            if (dim <= m_maxFullDimension)
            {
                return m_fullExplorer->isOver(dim);
            }
            else
            {
                return m_randExplorer->isOver(dim);
            }
        }

        /**
         * Returns the next generating values of dimension \c dim
         */ 
        typename ConstructionMethod::GenValue nextGenValue(unsigned int dim)
        {
            if (dim <= m_maxFullDimension)
            {
                return m_fullExplorer->nextGenValue(dim);
            }
            else
            {
                return m_randExplorer->nextGenValue(dim);
            } 
        }

        /**
         * Resets the explorer to the first dimension
         */ 
        void reset() 
        {
            m_randExplorer.reset();
            m_fullExplorer.reset();
        }

        /**
         * Sets the verbosity level of the explorer.
         */ 
        void setVerbose(int verbose)
        {
            m_randExplorer->setVerbose(verbose);
            m_fullExplorer->setVerbose(verbose);
        }

        /**
         * Switches the explorer to dimension \c dim.
         */ 
        void switchToDimension(unsigned int dim)
        {
            if (dim <= m_maxFullDimension)
            {
                return m_fullExplorer->switchToDimension(dim);
            }
            else
            {
                return m_randExplorer->switchToDimension(dim);
            } 
        }; 

    private:
        unsigned int m_dimension;
        unsigned int m_maxFullDimension;
        std::unique_ptr<RandomCBCExplorer<NC>> m_randExplorer;
        std::unique_ptr<FullCBCExplorer<NC>> m_fullExplorer;
};

}}

#endif
