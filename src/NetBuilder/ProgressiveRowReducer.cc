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

namespace NetBuilder{

        ProgressiveRowReducer::ProgressiveRowReducer():
        m_nCols(0),
        m_mat(0,0),
        m_rowOperations(0,0),
        m_pivotsColRowPositions(),
        m_pivotsRowColPositions(),
        m_columnsWithoutPivot(),
        m_rowsWithoutPivot()
    {};

    void ProgressiveRowReducer::reset(unsigned int nCols)  
    {
        m_nCols = nCols;
        m_nRows = 0;
        m_mat = GeneratingMatrix(0, m_nCols);
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

    void ProgressiveRowReducer::pivotRowAndFindNewPivot(unsigned int row)
    {
        for( const auto& rowColPivot : m_pivotsColRowPositions)
        {
            if (m_mat(row,rowColPivot.first)) // if required, use the row to flip this bit
            {
                m_rowOperations[row] = m_rowOperations[row] ^ m_rowOperations[rowColPivot.second];
                m_mat[row] = m_mat[row] ^ m_mat[rowColPivot.second];
            }
        }

        // std::cout << "4" << std::endl;
        unsigned int newPivotColPosition = m_nCols;
        for(std::set<unsigned int>::iterator it = m_columnsWithoutPivot.begin(); it != m_columnsWithoutPivot.end(); ++it)
        {
            if(m_mat(row, *it))
            {
                newPivotColPosition = *it;
                m_columnsWithoutPivot.erase(it); // this column will have a pivot
                break;
            }
        }
        // std::cout << "5" << std::endl;
        if (newPivotColPosition < m_nCols) // if such a pivot exists
        {
            
            m_pivotsColRowPositions[newPivotColPosition] = row;
            m_pivotsRowColPositions[row] = newPivotColPosition;
            for(unsigned int i = 0; i < row; ++i) // for each row above the inserted row
            {
                if(m_mat(i, newPivotColPosition)) // if required, use the row to flip this bit
                {
                    m_mat[i] = m_mat[i] ^ m_mat[row];
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[row];
                }
            }
        }
        else // if not
        {
            m_rowsWithoutPivot.push_back(row);
        }
    }


    void ProgressiveRowReducer::addRow(GeneratingMatrix newRow)
    {
    
        // std::cout << "1" << std::endl;
        unsigned int row = m_nRows;
        ++m_nRows;
        m_rowOperations.resize(m_nRows, m_nRows);
        m_rowOperations.flip(row,row);

        // std::cout << "2" << std::endl;
        m_mat.vstack(std::move(newRow));

        // std::cout << "3" << std::endl;

        pivotRowAndFindNewPivot(row);

    }

    void ProgressiveRowReducer::addColumn(GeneratingMatrix newCol)
    {
        newCol = m_rowOperations * newCol; // apply the row operations to the new column
        m_mat.stackRight(newCol); // stack right the new column
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

    void ProgressiveRowReducer::exchangeRow(unsigned int rowIndex, GeneratingMatrix newRow)
    {
        auto oldRowColPivotPosition = m_pivotsRowColPositions.find(rowIndex);
        if (oldRowColPivotPosition != m_pivotsRowColPositions.end())
        {
            unsigned int colPositionPivot = (*oldRowColPivotPosition).second;
            m_pivotsRowColPositions.erase(oldRowColPivotPosition);
            m_pivotsColRowPositions.erase(colPositionPivot);
            m_columnsWithoutPivot.insert(colPositionPivot);

            for(unsigned int i = 0; i < m_nRows; ++i)
            {
                if( i != rowIndex && m_rowOperations(i,rowIndex))
                {
                    m_mat[i] = m_mat[i] ^ m_mat[rowIndex];
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[rowIndex];
                }
            }
        }

        m_mat[rowIndex] = std::move(newRow[0]);
        for(unsigned int j = 0; j < m_nRows; ++j)
        {
            if (j != rowIndex)
            {
                m_rowOperations(rowIndex,j) = 0;
            }
        }

        pivotRowAndFindNewPivot(rowIndex);

    }
}
