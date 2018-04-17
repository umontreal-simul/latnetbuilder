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

#ifndef DIGITAL_NET_MATRIX_H
#define DIGITAL_NET_MATRIX_H
#include <boost/dynamic_bitset.hpp> 
#include <iostream>
#include <string>
#include <algorithm>
#include "latbuilder/Types.h"

// For now, only works in base 2, needs to be templated by the base.

namespace LatBuilder { namespace DigitalNet {

template<uInteger BASE>
class Matrix {

    public:

    Matrix(unsigned int n_rows, unsigned int n_cols, uInteger value = 0):
    m_data(n_rows*n_cols,0),
    m_rows(n_rows),
    m_cols(n_cols)
    {};

    unsigned int nCols() const { return m_cols; }

    unsigned int nRows() const { return m_rows; }


    const uInteger& operator()(unsigned int row, unsigned int column) const
    {
        return m_data[row*m_rows+column];
    }

    uInteger& operator()(unsigned int row, unsigned int column)
    {
        return m_data[row*m_rows+column];
    }

    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat)
    {
        for(unsigned int i = 0; i < mat.m_rows; ++i)
        {
            for(unsigned int j = 0; j < mat.n_columns; ++j)
            {
                os << mat(i,j) << " ";
            }
            os << std::endl;
        }
        return os; 
    }

    private:
        std::vector<uInteger> m_data;
        unsigned int m_rows;
        unsigned int m_cols;

};

template<>
class Matrix<2> {

    public:
        typedef boost::dynamic_bitset<> Row;
        typedef Row::reference reference;

        Matrix(unsigned int n_rows, unsigned int n_cols):
        m_data(n_rows,boost::dynamic_bitset<>(n_cols)),
        m_rows(n_rows),
        m_cols(n_cols)
        {
        };

        Matrix(unsigned int n_rows, unsigned int n_cols, std::vector<uInteger> init):
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

        Matrix(unsigned int n_rows, unsigned int n_cols, std::vector<string> init):
        m_data(n_rows),
        m_rows(n_rows),
        m_cols(n_cols)
        {
            assert(init.size() == m_rows);
            for(unsigned int i = 0; i < m_rows; ++i)
            {
                assert(init[i].length() == m_cols);
                std::reverse(init[i].begin(), init[i].end());
                m_data[i] = boost::dynamic_bitset<>(init[i]);
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
            for(unsigned int i = 0; i < m_rows; ++i)
            {
                bool tmp = m_data[i][col1];
                m_data[i][col1] = m_data[i][col2];
                m_data[i][col2] = tmp;
            }
        }

        friend std::ostream& operator<<(std::ostream& os, const Matrix& mat)
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

    private:
        std::vector<boost::dynamic_bitset<>> m_data;
        unsigned int m_rows;
        unsigned int m_cols;
};

}}
#endif