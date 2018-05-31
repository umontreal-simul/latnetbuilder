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

#ifndef NETBUILDER__TASK__RANDOM_CBC_EXPLORER_H
#define NETBUILDER__TASK__RANDOM_CBC_EXPLORER_H

#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"

#include "latbuilder/LFSR258.h"

#include <vector>
#include <algorithm>

namespace NetBuilder { namespace Task {

template <NetConstruction NC>
class RandomCBCExplorer
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:

        RandomCBCExplorer(unsigned int dimension, typename ConstructionMethod::DesignParameter designParameter, unsigned int nbTries, int verbose = 0):
            m_dimension(dimension),
            m_nbTries(nbTries),
            m_randomGenValueGenerator(std::move(designParameter)),
            m_countTries(dimension,0),
            m_verbose(verbose)
        {};

        bool isOver(unsigned int dim) 
        {
            return m_countTries[dim-1]>=m_nbTries;
        }

        typename ConstructionMethod::GenValue nextGenValue(unsigned int dim)
        {
            m_countTries[dim-1] += 1;
            if(this->m_verbose>0)
            {
                std::cout << "Dimension: " << dim << "/" << m_dimension <<  " - ";
                std::cout << "net " << m_countTries[dim-1] << "/" << m_nbTries << std::endl;
            }
            return m_randomGenValueGenerator(dim);
        }

        void reset()
        {
            std::fill(m_countTries.begin(),m_countTries.end(), 0);
        }

        void setVerbose(int verbose)
        {
            m_verbose = verbose;
        }

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
