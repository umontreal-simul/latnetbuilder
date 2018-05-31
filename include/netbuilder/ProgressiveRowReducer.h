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

#include <map>
#include <set>
#include <list>

namespace NetBuilder {

class ProgressiveRowReducer
{
    public:
        ProgressiveRowReducer();

        ProgressiveRowReducer copy();

        void reset(unsigned int nCols);
        
        void addRow(GeneratingMatrix newRow);

        void addColumn(GeneratingMatrix newCol);

        unsigned int exchangeRow(unsigned int rowIndex, GeneratingMatrix&& newRow, unsigned int smallestInvertible, int verbose);

        unsigned int computeRank() const;

        std::vector<unsigned int> computeRanks(unsigned int firstCol, unsigned int numCol) const;

        int computeSmallestInvertible(unsigned int firstCol, int numCol, unsigned int k);

        const GeneratingMatrix& matrix() const {return m_mat;}

        const GeneratingMatrix& rowOperations() const {return m_rowOperations; }

        void check();


    public:
        unsigned int m_nRows = 0;
        unsigned int m_nCols;
        GeneratingMatrix m_mat;
        GeneratingMatrix m_rowOperations;
        // GeneratingMatrix m_OriginalMatrix;
        // std::list<std::pair<unsigned int, unsigned int>> m_pivotsPositions;
        std::map<unsigned int, unsigned int> m_pivotsColRowPositions; // columns index are the keys and rows indexes are the values
        std::map<unsigned int, unsigned int> m_pivotsRowColPositions; // columns index are the keys and rows indexes are the values
        std::set<unsigned int> m_columnsWithoutPivot;
        std::list<unsigned int> m_rowsWithoutPivot;

        unsigned int pivotRowAndFindNewPivot(unsigned int row);

};

}

#endif