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
#include <numeric>

namespace NetBuilder{


    ProgressiveRowReducer::ProgressiveRowReducer():
        m_nCols(0),
        m_mat(0,0),
        m_rowOperations(0,0),
        m_permutation(0)
    {};

    void ProgressiveRowReducer::reset(unsigned int nCols)  
    {
        m_nCols = nCols;
        m_nRows = 0;
        m_mat = GeneratingMatrix(0, m_nCols);
        m_permutation.resize(m_nCols);
        std::iota(std::begin(m_permutation), std::end(m_permutation), 0);
        m_pivots.clear();
        m_rowOperations.resize(0,0);
        m_nullBlockStartingRow = 0;
    }

    bool ProgressiveRowReducer::reduceNewBlock(GeneratingMatrix block)
    {
        if (block.nCols() != m_nCols)
        {
            reset(block.nCols());
        }
    
        unsigned int oldNRows = m_nRows;
        m_nRows += block.nRows();
        m_pivots.resize(m_nRows,-1);
        m_rowOperations.resize(m_nRows, m_nRows);
        for(unsigned int i = oldNRows; i < m_nRows; ++i)
        {
            m_rowOperations(i,i) = 1;
        }
        block.permuteColumns(m_permutation);

        m_mat.vstack(std::move(block));

        for(unsigned int pivot = 0; pivot < m_nullBlockStartingRow; ++pivot)
        {
            unsigned int j = m_pivots[pivot];
            for(unsigned int row = oldNRows; row < m_nRows; ++ row )
            {
                if (m_mat(row,j))
                {
                    m_rowOperations[row] = m_rowOperations[row] ^ m_rowOperations[pivot];
                    m_mat[row] = m_mat[row] ^ m_mat[pivot];
                }
            }
        }
        
        unsigned int i_pivot = m_nullBlockStartingRow;

        unsigned int j;
        if (m_nullBlockStartingRow==0)
        {
            j = 0;
        }
        else
        {
            j = m_pivots[m_nullBlockStartingRow-1] + 1;
        }
        

        while (i_pivot < m_nRows && j < m_nCols){
            unsigned int i_temp = i_pivot;
            while (i_temp < m_nRows && !m_mat(i_temp,j)){
                i_temp++;
            }
            if (i_temp >= m_nRows){
                ++j;
                continue;
            }
            m_mat.swap_rows(i_temp, i_pivot);
            m_rowOperations.swap_rows(i_temp, i_pivot);
            m_pivots[i_pivot] = j;
            for (int i=i_pivot+1; i<m_nRows; i++){
                if (m_mat(i,j)){
                    m_mat[i] = m_mat[i] ^ m_mat[i_pivot];
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[i_pivot];
                }
            }
            ++j;
            i_pivot++;
        }

        m_nullBlockStartingRow = i_pivot;

        for(int i_pivot = m_nullBlockStartingRow-1; i_pivot >=0; --i_pivot)
        {
            unsigned int j = m_pivots[i_pivot];
            for (unsigned int i=0; i < i_pivot; i++){
                if (m_mat(i,j)){
                    m_mat[i] = m_mat[i] ^ m_mat[i_pivot];
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[i_pivot];
                }
            }
        }

        for (int i_pivot = 0; i_pivot < m_nullBlockStartingRow; i_pivot++)
        {
            unsigned int j = m_pivots[i_pivot];
            m_mat.swap_columns(i_pivot, j);
            //m_rowOperations.swap_columns(i_pivot, j);
            std::swap(m_permutation[i_pivot],m_permutation[j]);
            m_pivots[i_pivot] = i_pivot;
        }

        return m_nullBlockStartingRow == m_nRows;
    }

    bool ProgressiveRowReducer::addColumn(const GeneratingMatrix &col)
    {
        GeneratingMatrix newCol = m_rowOperations * col;
        m_mat.stackRight(newCol);
        m_permutation.push_back(m_nCols);
        ++m_nCols;

        unsigned int i_pivot = m_nullBlockStartingRow;
        while(i_pivot < m_nRows && !newCol(i_pivot,0))
        {
            ++i_pivot;
        }
        if (i_pivot < m_nRows)
        {
            assert(m_pivots[i_pivot]==-1);
            m_pivots[i_pivot] = m_nCols-1;
            m_mat.swap_rows(m_nullBlockStartingRow, i_pivot);
            m_rowOperations.swap_rows(m_nullBlockStartingRow, i_pivot);
            std::swap(m_pivots[i_pivot], m_pivots[m_nullBlockStartingRow]);
            i_pivot = m_nullBlockStartingRow;
            ++m_nullBlockStartingRow;
            
            unsigned int newColPos = m_nCols-1;

            if (m_nCols>=2)
            {
                m_mat.swap_columns(m_nCols-1, m_nCols-2);

                //m_rowOperations.swap_columns(m_nCols-1, m_nCols-2);
                std::swap(m_permutation[m_nCols-1],m_permutation[m_nCols-2]);
                newColPos = m_nCols-2;
                m_pivots[i_pivot] = m_nCols-2;
            }
            for(unsigned int i = 0; i < i_pivot; ++i)
            {
                if(m_mat(i,newColPos))
                {
                    m_mat[i] = m_mat[i] ^ m_mat[i_pivot];
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[i_pivot];
                }
            }
            for(unsigned int i = i_pivot+1; i < m_nRows; ++i)
            {
                if(m_mat(i,newColPos))
                {
                    m_mat[i] = m_mat[i] ^ m_mat[i_pivot];
                    m_rowOperations[i] = m_rowOperations[i] ^ m_rowOperations[i_pivot];
                }
            }
        }
        return m_nullBlockStartingRow == m_nRows;;
    }
}
