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

#include "netbuilder/ProgressiveRowReducer.h"

#include <algorithm>
#include <iterator>

namespace NetBuilder{

    ProgressiveRowReducer::ProgressiveRowReducer(unsigned int nCols)
    {
        reset(nCols);
    };

    void ProgressiveRowReducer::reset(unsigned int nCols)  
    {
        m_nCols = nCols;
        m_nRows = 0;
        m_mat = GeneratingMatrix(0, m_nCols);
        // m_OriginalMatrix = GeneratingMatrix(0, m_nCols);
        m_columnsWithoutPivot.clear();
        for(unsigned int j = 0; j < nCols; ++j)
        {
            m_columnsWithoutPivot.insert(j); // TO DO
        }
        m_rowsWithoutPivot.clear();
        m_pivotsColRowPositions.clear();
        m_pivotsRowColPositions.clear();
        m_rowOperations.resize(0,m_nCols);
    }

    unsigned int ProgressiveRowReducer::computeRank() const
    {
        return (unsigned int) m_pivotsColRowPositions.size();
    }

    std::vector<unsigned int> ProgressiveRowReducer::computeRanks(unsigned int firstCol, unsigned int numCol) const
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

    int ProgressiveRowReducer::computeSmallestInvertible(unsigned int k){
        if (m_pivotsColRowPositions.size()<k){
            return m_nCols;
        }
        else{
            return (*std::max_element(m_pivotsColRowPositions.begin(), m_pivotsColRowPositions.end())).first;
        }
    }

    unsigned int ProgressiveRowReducer::pivotRowAndFindNewPivot(unsigned int rowIndex)
    {

        for( const auto& colRowPivot : m_pivotsColRowPositions)
        {
            
            if (m_mat(rowIndex,colRowPivot.first)) // if required, use the pivot to flip this bit
            {
                m_rowOperations[rowIndex] = m_rowOperations[rowIndex] ^ m_rowOperations[colRowPivot.second];
                m_mat[rowIndex] = m_mat[rowIndex] ^ m_mat[colRowPivot.second];
            }
        }

        unsigned int newPivotColPosition = m_nCols;
        for(std::set<unsigned int>::iterator it = m_columnsWithoutPivot.begin(); it != m_columnsWithoutPivot.end(); ++it)
        {
            if(m_mat(rowIndex, *it))
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
                if(i != rowIndex && m_mat(i, newPivotColPosition)) // if required, use the rowIndex to flip this bit
                {
                    m_mat[i] = m_mat[i] ^ m_mat[rowIndex];
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


    void ProgressiveRowReducer::addRow(GeneratingMatrix newRow)
    {
        unsigned int row = m_nRows;
        ++m_nRows;
        m_rowOperations.resize(m_nRows, m_nRows);
        m_rowOperations.flip(row,row);

        m_mat.vstack(std::move(newRow));
        // m_OriginalMatrix.vstack(newRow);
        // m_mat.vstack(newRow);

        pivotRowAndFindNewPivot(row);

    }

    void ProgressiveRowReducer::addColumn(GeneratingMatrix newCol)
    {
        newCol = m_rowOperations * newCol; // apply the row operations to the new column
        m_mat.stackRight(newCol); // stack right the new column
        // m_OriginalMatrix.stackRight(newCol);
        unsigned int col = m_nCols;
        ++m_nCols;

        unsigned int newPivotRowPosition = m_nRows;
        for(std::list<unsigned int>::iterator it = m_rowsWithoutPivot.begin(); it != m_rowsWithoutPivot.end(); ++it)
        {
            if(m_mat(*it,col))
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
                if( i != newPivotRowPosition && m_mat(i,col))
                {
                    m_mat.flip(i,col);
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[newPivotRowPosition];
                }
            }
        }
        else
        {
            m_columnsWithoutPivot.insert(col);
        }
    }

    unsigned int ProgressiveRowReducer::exchangeRow(unsigned int rowIndex, GeneratingMatrix&& newRow, unsigned int smallestInvertible, int verbose=0)
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
                        m_mat.swap_rows(tmpIndex, rowIndex);
                        m_rowOperations.swap_rows(tmpIndex, rowIndex);

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
                    m_mat[i] = m_mat[i] ^ m_mat[rowIndex];
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[rowIndex];
                }
            }
        }

        m_mat[rowIndex] = std::move(newRow[0]);
        // m_mat[rowIndex] = newRow[0];
        // m_OriginalMatrix[rowIndex] = newRow[0];

        m_rowOperations[rowIndex].reset();
        m_rowOperations(rowIndex, rowIndex) = 1;

        unsigned int newPivotPos = pivotRowAndFindNewPivot(rowIndex);

        return std::max(smallestInvertible, newPivotPos);
    }

void first_pivot(GeneratingMatrix M, int verbose= 0){
    int k = M.nRows();
    int m = M.nCols();
    
    int i_pivot=0;
    int j=-1;
    int Pivots[k];
    for (int i=0; i<k; i++){
        Pivots[i] = -1;
    }
    
    while (i_pivot < k && j < m-1){
        j++;
        int i_temp = i_pivot;
        while (i_temp < k && M[i_temp][j] == 0){
            i_temp++;
        }
        if (i_temp >= k){  // pas d'element non nul sur la colonne
            continue;
        }
        M.swap_rows(i_temp, i_pivot);

        Pivots[i_pivot] = j;
        for (int i=i_pivot+1; i<k; i++){
            if (M[i][j] != 0){
                M[i] = M[i] ^ M[i_pivot];
            }
        }
        i_pivot++;
    }
    if (Pivots[k-1] == -1){
        // assert(false);
        throw std::runtime_error("rowOperations not invertible");
    }
}

void ProgressiveRowReducer::check(){
    first_pivot(m_rowOperations);

    std::vector<bool> check_row (m_nRows, 0);
    std::vector<bool> check_col (m_nCols, 0);


    // GeneratingMatrix prod = m_rowOperations * m_OriginalMatrix;
    // for (int i=0; i < m_nRows; i++){
    //     for (int j=0; j < m_nCols; j++){
    //         // std::cout << "checking prod" << std::endl;
    //         if (prod(i, j) != m_mat(i, j)){
    //             throw std::runtime_error("checking prod");
    //         }
    //     }
    // }
    for (const auto& colRow : m_pivotsColRowPositions){
        unsigned int col = colRow.first;
        unsigned int row = colRow.second;
        for (unsigned int i=0; i < m_nRows; i++){
            if (m_mat(i, col) != (i == row)){
                throw std::runtime_error("checking pivot");
            }
        }
        check_row[row] = 1;
        check_col[col] = 1;
    }

    for (const auto& rowCol : m_pivotsRowColPositions){
        unsigned int row = rowCol.first;
        unsigned int col = rowCol.second;
        if (check_row[row] != 1 || check_col[col] != 1){
            throw std::runtime_error("checking row col 1");
        }
        if (m_pivotsColRowPositions[col] != row){
            throw std::runtime_error("checking rowCol and colRow compatible");
        }
    }
    if (m_pivotsColRowPositions.size() != m_pivotsRowColPositions.size()){
        throw std::runtime_error("checking rowCol and colRow size-compatible");
    }

    for (const auto& col: m_columnsWithoutPivot){
        if (check_col[col] != 0){
            throw std::runtime_error("checking row col 2");
        }
        check_col[col] = 1;
    }
    for (const auto& row: m_rowsWithoutPivot){
        if (check_row[row] != 0){
            throw std::runtime_error("checking row col 3");
        }
        check_row[row] = 1;
    }

    for (const auto& r: check_row){
        if(r != 1){
            throw std::runtime_error("checking row col 4");
        }
    }
    for (const auto& c : check_col){
        if(c != 1){
            throw std::runtime_error("checking row col 5");
        }
    }

}

}
