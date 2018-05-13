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

#ifndef NETBUILDER__LEVEL_COMBINER_H
#define NETBUILDER__LEVEL_COMBINER_H

#include <netbuilder/Types.h>
#include "netbuilder/Util.h"
#include <limits>
#include <boost/numeric/ublas/blas.hpp>

namespace NetBuilder { 

    struct MaxCombiner
    {
        Real operator()(const RealVector& merits){
            return boost::numeric::ublas::blas_1::amax<RealVector>(merits);
        }
    };

    struct SumCombiner
    {
        Real operator()(const RealVector& merits){
            Real res = 0;
            for(Real merit: merits)
            {
                res+= merit;
            }
            return res;
        }
    };

    struct JoeKuoD6Combiner
    {
        Real operator()(const RealVector& merits){
            Real res = 0;
            unsigned int level = 0;
            for(Real merit : merits)
            {
                ++level;
                res = std::max(res, intPow(merit,6)/(level-merit+1));
            }
            return res;
        }
    };

    class LevelSelectorCombiner
    {
        public:

            LevelSelectorCombiner(unsigned int level):
                m_level(level)
            {};

            Real operator()(const RealVector& merits)
            {
                return merits[m_level-1];
            }

        private:   
            unsigned int m_level;
    };
}

#endif