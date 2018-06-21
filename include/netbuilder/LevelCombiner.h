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

#ifndef NETBUILDER__LEVEL_COMBINER_H
#define NETBUILDER__LEVEL_COMBINER_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include <limits>
#include <boost/numeric/ublas/blas.hpp>

namespace NetBuilder { 

    namespace LevelCombiner {

        /** 
         * Maximum combiner. Combines merits using the maximum of the merits.
         */ 
        struct MaxCombiner
        {
            /** 
             * Combines the merits.
             */ 
            Real operator()(const RealVector& merits)
            {
                Real res = merits[0];
                for(unsigned int i = 1; i < merits.size(); ++i)
                {
                    res = std::max(res, merits[i]);
                }
                return res;
            }
        };

        /**
         * Sum combiner. Combines merits using the sum of the merits.
         */ 
        struct SumCombiner
        {
            /** 
             * Combines the merits.
             */ 
            Real operator()(const RealVector& merits)
            {
                Real res = 0;
                for(Real merit: merits)
                {
                    res+= merit;
                }
                return res;
            }
        };

        /**
         * Level selector combiner. Selects the merit value corresponding to a specific level.
         */ 
        class LevelSelectorCombiner
        {
            public:

                /** Constructs a combiner selecting the merit corresponding to \c level.
                 * @param level Level to select. Starts from 1. 
                 */ 
                LevelSelectorCombiner(unsigned int level):
                    m_level(level)
                {};

                /** 
                * Combines the merits.
                */ 
                Real operator()(const RealVector& merits)
                {
                    return merits[m_level-1];
                }

            private:   
                unsigned int m_level;
        };
}}

#endif