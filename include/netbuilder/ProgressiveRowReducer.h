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

#ifndef NETBUILDER__PROGRESSIVE_ROW_REDUCER_H
#define NETBUILDER__PROGRESSIVE_ROW_REDUCER_H

#include "netbuilder/GeneratingMatrix.h"

#include <map>
#include <set>
#include <list>

// #define DEBUG_ROW_REDUCER

namespace NetBuilder {

/**
 * Class used to perform row reduction operations on a matrix.
 */ 
class ProgressiveRowReducer
{
    public:
        /** Constructor.
         * @param nCols number of columns of the reducer.
         */ 
        ProgressiveRowReducer(unsigned int nCols = 0);

        /**
         * Clears the reducer and set the number of columns to \c nCols.
         * @param nCols New number of columns of the reducer.
         */ 
        void reset(unsigned int nCols);
        
        /**
         * Adds a row below the current matrix and updates the reduction subsequently.
         * @param newRow The one-row matrix to stack below.
         */ 
        void addRow(GeneratingMatrix newRow);

        /**
         * Adds a column on the right to the current matrix and updates the reduction subsequently.
         * @param newCol The one-column matrix to stack on the right.
         */ 
        void addColumn(GeneratingMatrix newCol);

        /**
         * Exchanges the row in position \c rowIndex with \c newRow.
         * @param rowIndex
         * @param newRow
         * @param smallestInvertible Index of the last column of the smallest full-row-rank submatrix.
         * @param verbose Verbosity level.
         */ 
        unsigned int exchangeRow(unsigned int rowIndex, GeneratingMatrix&& newRow, unsigned int smallestInvertible, int verbose = 0);

        /** 
         * Computes the rank of the matrix.
         */ 
        unsigned int computeRank() const;

        /** 
         * Computes the ranks of the submatrices with an increasing number of columns.
         * @param firstCol Index of the the last column of the first submatrix.
         * @param numCol Number of submatrices to consider.
         */ 
        std::vector<unsigned int> computeRanks(unsigned int firstCol, unsigned int numCol) const;

        /** 
         * Computes the smallest column index such that the submatrix whose last column corresponds to this index
         * is invertible.
         * @param firstCol
         * @param numCol
         * @param k Number of rows.
         */ 
        int computeSmallestInvertible(unsigned int firstCol, int numCol, unsigned int k);

        /**
         * Returns a const reference to the row-reduced matrix.
         */ 
        const GeneratingMatrix& matrix() const {return m_redMat;}

        /**
         * Returns a const reference to the row operations matrix.
         */ 
        const GeneratingMatrix& rowOperations() const {return m_rowOperations; }

        /**
         * Returns the number of rows in the reducer.
         */ 
        unsigned int numRows() const {return m_nRows; }

        /**
         * Returns the number of columns in the reducer.
         */ 
        unsigned int numCols() const {return m_nCols; }

        /**
         * Check if a matrix is invertible. Returns false if the matrix is not-square or singular, 
         * and true otherwise.
         */ 
        static bool checkIfInvertible(GeneratingMatrix matrix) ;
        
        #ifdef DEBUG_ROW_REDUCER
        void check();
        #endif


    private:
        unsigned int m_nRows = 0; // number of rows in the reducer
        unsigned int m_nCols; // number of columns of the reducer
        GeneratingMatrix m_redMat; // row-reduced matrix
        GeneratingMatrix m_rowOperations; // row operations matrix
        #ifdef DEBUG_ROW_REDUCER
        GeneratingMatrix m_baseMatrix;
        #endif
        std::map<unsigned int, unsigned int> m_pivotsColRowPositions; // columns index are the keys and rows indexes are the values
        std::map<unsigned int, unsigned int> m_pivotsRowColPositions; // columns index are the keys and rows indexes are the values
        std::set<unsigned int> m_columnsWithoutPivot; // ordered set for columns without a pivot
        std::list<unsigned int> m_rowsWithoutPivot; // list of rows without a pivot 

        /**
         * Uses existing pivots to pivot the row at position \c rowIndex and look for
         * a new pivot on this row. If such a pivot exists, uses it to pivot the other rows.
         * @param rowIndex Index of the row.
         */ 
        unsigned int pivotRowAndFindNewPivot(unsigned int rowIndex);

};

}

#endif