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

#include "netbuilder/Helpers/RankComputer.h"

#include <algorithm>
#include <iterator>

namespace NetBuilder{

    RankComputer::RankComputer(unsigned int nCols)
    {
        reset(nCols);
    };

    void RankComputer::reset(unsigned int nCols)  
    {
        m_nCols = nCols;
        m_nRows = 0;
        m_smallestFullRank = nCols;
        m_redMat = GeneratingMatrix(0, m_nCols);
        #ifdef DEBUG_ROW_REDUCER
        m_baseMatrix = GeneratingMatrix(0, m_nCols);
        #endif
        m_columnsWithoutPivot.clear();
        for(unsigned int j = 0; j < nCols; ++j)
        {
            m_columnsWithoutPivot.insert(m_columnsWithoutPivot.end(), j);
        }
        m_rowsWithoutPivot.clear();
        m_pivotsColRowPositions.clear();
        m_pivotsRowColPositions.clear();
        m_rowOperations.resize(0,m_nCols);
    }

    unsigned int RankComputer::computeRank() const
    {
        return (unsigned int) m_pivotsColRowPositions.size();
    }

    std::vector<unsigned int> RankComputer::computeRanks(unsigned int firstCol, unsigned int numCol) const
    {
        unsigned int rank = 0;
        std::vector<unsigned int> ranks(numCol, rank);
        unsigned int lastCol = firstCol;

        for(const auto& colRow : m_pivotsColRowPositions)
        {
            if(colRow.first >= firstCol+numCol)
            {
                break;
            }

            for(unsigned int col = lastCol; col < colRow.first ; ++col)
            {
                ranks[col-firstCol] = rank;
            }

            rank+=1;

            if (colRow.first >= firstCol)
            {
                lastCol = colRow.first;
            }
        }

        for(unsigned int col = lastCol; col < firstCol + numCol; ++col)
        {
            ranks[col-firstCol] = rank;
        }

        return ranks;
    }

    unsigned int RankComputer::pivotRowAndFindNewPivot(unsigned int rowIndex)
    {

        for( const auto& colRowPivot : m_pivotsColRowPositions)
        {
            
            if (m_redMat(rowIndex,colRowPivot.first)) // if required, use the pivot to flip this bit
            {
                m_rowOperations[rowIndex] = m_rowOperations[rowIndex] ^ m_rowOperations[colRowPivot.second];
                m_redMat[rowIndex] = m_redMat[rowIndex] ^ m_redMat[colRowPivot.second];
            }
        }

        unsigned int newPivotColPosition = m_nCols;
        for(std::set<unsigned int>::iterator it = m_columnsWithoutPivot.begin(); it != m_columnsWithoutPivot.end(); ++it)
        {
            if(m_redMat(rowIndex, *it))
            {
                newPivotColPosition = *it;
                m_columnsWithoutPivot.erase(it); // this column will have a pivot
                break;
            }
        }

        if (newPivotColPosition < m_nCols) // if such a pivot exists
        {
            
            m_pivotsColRowPositions[newPivotColPosition] = rowIndex;
            m_pivotsRowColPositions[rowIndex] = newPivotColPosition;
            for(unsigned int i = 0; i < m_nRows; ++i) // for each rowIndex above the inserted rowIndex
            {
                if(i != rowIndex && m_redMat(i, newPivotColPosition)) // if required, use the rowIndex to flip this bit
                {
                    m_redMat[i] = m_redMat[i] ^ m_redMat[rowIndex];
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[rowIndex];
                }
            }
            
        }
        else // if not
        {
            m_rowsWithoutPivot.push_back(rowIndex);
        }
        return newPivotColPosition;
    }


    void RankComputer::addRow(GeneratingMatrix newRow)
    {
        unsigned int row = m_nRows;
        ++m_nRows;
        m_rowOperations.resize(m_nRows, m_nRows);
        m_rowOperations.flip(row,row);

        #ifdef DEBUG_ROW_REDUCER
        m_baseMatrix.stackBelow(newRow);
        m_redMat.stackBelow(newRow);
        #else
        m_redMat.stackBelow(std::move(newRow));
        #endif

        pivotRowAndFindNewPivot(row);

        if (m_pivotsColRowPositions.size() < m_nRows)
        {
            m_smallestFullRank = m_nCols + 1;
        }
        else
        {
            m_smallestFullRank = (*std::max_element(m_pivotsColRowPositions.begin(), m_pivotsColRowPositions.end())).first + 1;
        }

    }

    void RankComputer::addColumn(GeneratingMatrix newCol)
    {
        newCol = m_rowOperations * newCol; // apply the row operations to the new column
        m_redMat.stackRight(newCol); // stack right the new column

        #ifdef DEBUG_ROW_REDUCER
        m_baseMatrix.stackRight(newCol);
        #endif

        unsigned int col = m_nCols;
        ++m_nCols;

        unsigned int newPivotRowPosition = m_nRows;
        for(std::list<unsigned int>::iterator it = m_rowsWithoutPivot.begin(); it != m_rowsWithoutPivot.end(); ++it)
        {
            if(m_redMat(*it,col))
            {
                newPivotRowPosition = *it;
                m_rowsWithoutPivot.erase(it); // this row will have a pivot
                break;
            }
        }

        if(newPivotRowPosition < m_nRows)
        {
            m_pivotsColRowPositions[col] = newPivotRowPosition;
            m_pivotsRowColPositions[newPivotRowPosition] = col;

            for(unsigned int i = 0; i < m_nRows; ++i)
            {
                if( i != newPivotRowPosition && m_redMat(i,col))
                {
                    m_redMat.flip(i,col);
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[newPivotRowPosition];
                }
            }
        }
        else
        {
            m_columnsWithoutPivot.insert(col);
        }
    }

    void RankComputer::replaceRow(unsigned int rowIndex, GeneratingMatrix&& newRow, int verbose)
    {
        auto rowIndexColPivPos = m_pivotsRowColPositions.find(rowIndex);

        if (rowIndexColPivPos != m_pivotsRowColPositions.end())
        {
            unsigned int colPositionPivot = (*rowIndexColPivPos).second;
            unsigned int firstRowToDepivot = 0;
            if (m_rowOperations(rowIndex, rowIndex) != 1){
                for(unsigned int tmpIndex = 0; tmpIndex < m_nRows; ++tmpIndex)
                {
                    if(m_rowOperations(tmpIndex,rowIndex)){
                        m_redMat.swapRows(tmpIndex, rowIndex);
                        m_rowOperations.swapRows(tmpIndex, rowIndex);

                        auto tmpIndexPivPos = m_pivotsRowColPositions.find(tmpIndex);
                        int tmpIndexColPivPos;
                        if(tmpIndexPivPos != m_pivotsRowColPositions.end())
                        {
                            tmpIndexColPivPos = (*tmpIndexPivPos).second;
                            m_pivotsColRowPositions.erase(tmpIndexColPivPos);
                            m_columnsWithoutPivot.insert(tmpIndexColPivPos);
                        }
                        
                        m_pivotsRowColPositions.erase(rowIndex);
                        m_pivotsColRowPositions[colPositionPivot] = tmpIndex;
                        m_pivotsRowColPositions[tmpIndex] = colPositionPivot;
                        
                        firstRowToDepivot = tmpIndex+1;
                        break;
                    }
                }
            }
            else{
                m_pivotsRowColPositions.erase(rowIndex);
                m_pivotsColRowPositions.erase(colPositionPivot);
                m_columnsWithoutPivot.insert(colPositionPivot);
            }

            for(unsigned int i = firstRowToDepivot; i < m_nRows; ++i)
            {
                if(i!=rowIndex && m_rowOperations(i,rowIndex))
                {
                    m_redMat[i] = m_redMat[i] ^ m_redMat[rowIndex];
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[rowIndex];
                }
            }
        }

        #ifdef DEBUG_ROW_REDUCER
        m_redMat[rowIndex] = newRow[0];
        m_baseMatrix[rowIndex] = newRow[0];
        #else
        m_redMat[rowIndex] = std::move(newRow[0]);
        #endif

        m_rowOperations[rowIndex].reset();
        m_rowOperations(rowIndex, rowIndex) = 1;

        unsigned int newPivotPos = pivotRowAndFindNewPivot(rowIndex);

        m_smallestFullRank = std::max(m_smallestFullRank, newPivotPos + 1);
    }

    bool RankComputer::checkIfInvertible(GeneratingMatrix matrix)
    {
        int k = matrix.nRows();
        int m = matrix.nCols();

        if (k != m)
        {
            return false;
        }
        
        int i_pivot=0;
        int j=-1;
        int Pivots[k];
        for (int i=0; i<k; i++){
            Pivots[i] = -1;
        }
        
        while (i_pivot < k && j < m-1){
            j++;
            int i_temp = i_pivot;
            while (i_temp < k && matrix[i_temp][j] == 0){
                i_temp++;
            }
            if (i_temp >= k){  // pas d'element non nul sur la colonne
                continue;
            }
            matrix.swapRows(i_temp, i_pivot);

            Pivots[i_pivot] = j;
            for (int i=i_pivot+1; i<k; i++){
                if (matrix[i][j] != 0){
                    matrix[i] = matrix[i] ^ matrix[i_pivot];
                }
            }
            i_pivot++;
        }

        return Pivots[k-1] != -1;
    }

#ifdef DEBUG_ROW_REDUCER
void RankComputer::check(){

    if (!checkIfInvertible(m_rowOperations))
    {
        throw std::runtime_error("Row operations matrix is not invertible.")
    }

    std::vector<bool> check_row (m_nRows, 0);
    std::vector<bool> check_col (m_nCols, 0);

    GeneratingMatrix prod = m_rowOperations * m_baseMatrix;
    for (int i=0; i < m_nRows; i++){
        for (int j=0; j < m_nCols; j++){
            if (prod(i, j) != m_redMat(i, j)){
                throw std::runtime_error("The left-product of the base matrix by the row-operations matrix does not correspond to the reduced matrix.s");
            }
        }
    }

    for (const auto& colRow : m_pivotsColRowPositions){
        unsigned int col = colRow.first;
        unsigned int row = colRow.second;
        for (unsigned int i=0; i < m_nRows; i++){
            if (m_redMat(i, col) != (i == row)){
                throw std::runtime_error("A column containing a pivot has not the good property.");
            }
        }
        check_row[row] = 1;
        check_col[col] = 1;
    }

    for (const auto& rowCol : m_pivotsRowColPositions){
        unsigned int row = rowCol.first;
        unsigned int col = rowCol.second;
        if (check_row[row] != 1 || check_col[col] != 1){
            throw std::runtime_error("RowCol and ColRow maps are incompatible (1) .");
        }
        if (m_pivotsColRowPositions[col] != row){
            throw std::runtime_error("RowCol and ColRow maps are incompatible (2) .");
        }
    }
    if (m_pivotsColRowPositions.size() != m_pivotsRowColPositions.size()){
        throw std::runtime_error("RowCol and ColRow maps are incompatible (3) .");
    }

    for (const auto& col: m_columnsWithoutPivot){
        if (check_col[col] != 0){
            throw std::runtime_error("Column without pivot in pivot map.");
        }
        check_col[col] = 1;
    }
    for (const auto& row: m_rowsWithoutPivot){
        if (check_row[row] != 0){
            throw std::runtime_error("Row without pivot in pivot map.");
        }
        check_row[row] = 1;
    }

    for (const auto& r: check_row){
        if(r != 1){
            throw std::runtime_error("Duplicate or missing row.");
        }
    }
    for (const auto& c : check_col){
        if(c != 1){
            throw std::runtime_error("Duplicate or missing column.");
        }
    }
}
#endif

}
