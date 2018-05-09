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

/**
 * \file
 * Tools for streaming and poor man's factorization.
 */

#ifndef NETBUILDER__PROGRESSIVE_ROW_REDUCER_H
#define NETBUILDER__PROGRESSIVE_ROW_REDUCER_H

#include "netbuilder/GeneratingMatrix.h"

namespace NetBuilder {

class ProgressiveRowReducer
{
    public:
        ProgressiveRowReducer();

        void reset(unsigned int nCols);
        
        bool reduceNewBlock(GeneratingMatrix block);

        bool addColumn(const GeneratingMatrix &col);

        const GeneratingMatrix& matrix() const {return m_mat;}

        const GeneratingMatrix& rowOperations() const {return m_rowOperations; }

    public:
        unsigned int m_nRows = 0;
        unsigned int m_nCols;
        GeneratingMatrix m_mat;
        GeneratingMatrix m_rowOperations;
        std::vector<unsigned int> m_permutation;
        std::vector<int> m_pivots;
        unsigned int m_nullBlockStartingRow = 0;

};

}

#endif