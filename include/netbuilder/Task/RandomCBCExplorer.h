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
template <NetConstruction NC>
class RandomCBCExplorer
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:

        /** Constructor.
         * @param dimension Number of coordinates of the explorer.
         * @param designParameter Design parameter of the search space.
         * @param nbTries Number of random choices of generating values by dimension.
         * @param verbose Verbosity level.
         */
        RandomCBCExplorer(unsigned int dimension, typename ConstructionMethod::DesignParameter designParameter, unsigned int nbTries, int verbose = 0):
            m_dimension(dimension),
            m_nbTries(nbTries),
            m_randomGenValueGenerator(std::move(designParameter)),
            m_countTries(dimension,0),
            m_verbose(verbose)
        {};

        /**
         * Returns whether the dimension \c dim is fully explored
         */ 
        bool isOver(unsigned int dim) 
        {
            return m_countTries[dim]>=m_nbTries;
        }

        /**
         * Returns the next generating values of dimension \c dim
         */
        typename ConstructionMethod::GenValue nextGenValue(unsigned int dim)
        {
            m_countTries[dim] += 1;
            if(this->m_verbose>0)
            {
                std::cout << "Dimension: " << dim << "/" << m_dimension <<  " - ";
                std::cout << "net " << m_countTries[dim] << "/" << m_nbTries << std::endl;
            }
            return m_randomGenValueGenerator(dim);
        }


        /**
         * Resets the explorer to the first dimension
         */ 
        void reset()
        {
            std::fill(m_countTries.begin(),m_countTries.end(), 0);
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
        {};

    private:
        unsigned int m_dimension;
        unsigned int m_nbTries;
        typename ConstructionMethod:: template RandomGenValueGenerator <LatBuilder::LFSR258> m_randomGenValueGenerator;
        std::vector<unsigned int> m_countTries;
        int m_verbose;

};

}}

#endif
