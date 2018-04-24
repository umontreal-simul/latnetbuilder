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

#ifndef NET_BUILDER__MATRIX_H
#define NET_BUILDER__MATRIX_H
#include <boost/dynamic_bitset.hpp> 
#include <iostream>
#include <string>
#include <algorithm>
#include "netbuilder/Types.h"


namespace NetBuilder {

class GeneratingMatrix {

    public:
        typedef boost::dynamic_bitset<> Row;
        typedef Row::reference reference;

        GeneratingMatrix(unsigned int n_rows, unsigned int n_cols):
        m_data(n_rows,boost::dynamic_bitset<>(n_cols)),
        m_rows(n_rows),
        m_cols(n_cols)
        {
        };

        GeneratingMatrix(unsigned int n_rows, unsigned int n_cols, std::vector<uInteger> init):
        m_data(n_rows),
        m_rows(n_rows),
        m_cols(n_cols)
        {
            assert(init.size() == m_rows);
            for(unsigned int i = 0; i < m_rows; ++i)
            {
                m_data[i] = boost::dynamic_bitset<>(n_cols,init[i]);
            }
        };

        GeneratingMatrix(unsigned int n_rows, unsigned int n_cols, bool random):
        m_data(n_rows),
        m_rows(n_rows),
        m_cols(n_cols)
        {
            std::string str;
            for (int i=0; i<m_cols; i++){
                str = "";
                for (int j=0; j<m_cols; j++){
                    if (rand() % 2 == 0){
                        str.append("0");
                    }
                    else{
                        str.append("1");
                    }
                }
                m_data[i] = boost::dynamic_bitset<>(str);
            }
        };

        unsigned int nCols() const { return m_cols; }

        unsigned int nRows() const { return m_rows; }

        Row operator[](unsigned int row) const
        {
            return m_data[row];
        }

        Row& operator[](unsigned int row)
        {
            return m_data[row];
        }

        bool operator()(unsigned int row, unsigned int column) const
        {
            return m_data[row][column];
        }

        reference operator()(unsigned int row, unsigned int column)
        {
            return m_data[row][column];
        }

        void flip(unsigned int row, unsigned int column)
        {
            m_data[row].flip(column);
        }

        void swap_rows(unsigned int row1, unsigned int row2){
            std::swap(m_data[row1], m_data[row2]);
        }

        void swap_columns(unsigned int col1, unsigned int col2)
        {
            assert(col1 < m_cols && col2 < m_cols);
            for(unsigned int i = 0; i < m_rows; ++i)
            {
                bool tmp = m_data[i][col1];
                m_data[i][col1] = m_data[i][col2];
                m_data[i][col2] = tmp;
            }
        }

        friend std::ostream& operator<<(std::ostream& os, const GeneratingMatrix& mat)
        {
            for(unsigned int i = 0; i < mat.m_rows; ++i)
            {
                for(unsigned int j = 0; j < mat.m_cols; ++j)
                {
                    os << mat(i,j) << " ";
                }
                os << std::endl;
            }
            return os; 
        }

        
        template<typename RAND>
        static GeneratingMatrix randomMatrix(unsigned int n_rows, unsigned int n_cols, RAND& randomGen, bool uniTriangular = true)
        {
            GeneratingMatrix res(n_rows,n_cols);

            if (uniTriangular)
            {
                for(unsigned int i = 0; i < n_rows; ++i)
                {
                    if (i < n_cols)
                    {
                        res(i,i) = 1;
                    }
                    for(unsigned int j = i+1; j < n_cols; ++j)
                    {
                        res(i,j) = randomGen() % 2;
                    }
                }
            }
            else
            {
                for(unsigned int i = 0; i < n_rows; ++i)
                {
                    for(unsigned int j = 0; j < n_cols; ++j)
                    {
                        res(i,j) = randomGen() % 2;
                    }
                }
            }

            return res;
        };

    private:
        std::vector<boost::dynamic_bitset<>> m_data;
        unsigned int m_rows;
        unsigned int m_cols;
};



}
#endif