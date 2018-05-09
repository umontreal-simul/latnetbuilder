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

#include "netbuilder/GeneratingMatrix.h"

namespace NetBuilder {

GeneratingMatrix::GeneratingMatrix(unsigned int n_rows, unsigned int n_cols):
    m_data(n_rows,boost::dynamic_bitset<>(n_cols)),
    m_rows(n_rows),
    m_cols(n_cols)
{};

GeneratingMatrix::GeneratingMatrix(unsigned int n_rows, unsigned int n_cols, std::vector<uInteger> init):
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

GeneratingMatrix::GeneratingMatrix(unsigned int n_rows, unsigned int n_cols, bool uniTriangular):
    GeneratingMatrix(n_rows,n_cols)
{
    if (uniTriangular)
    {
        for(unsigned int i = 0; i < n_rows; ++i)
        {
            if (i < n_cols)
            {
                (*this)(i,i) = 1;
            }
            for(unsigned int j = i+1; j < n_cols; ++j)
            {
                (*this)(i,j) = rand() % 2;
            }
        }
    }
    else
    {
        for(unsigned int i = 0; i < n_rows; ++i)
        {
            for(unsigned int j = 0; j < n_cols; ++j)
            {
                (*this)(i,j) = rand() % 2;
            }
        }
    }
}

unsigned int GeneratingMatrix::nCols() const { return m_cols; }

unsigned int GeneratingMatrix::nRows() const { return m_rows; }

std::vector<unsigned long> GeneratingMatrix::getColsReverse() const{
    std::vector<unsigned long> res(nCols(), 0);
    for (unsigned int j=0; j<nCols(); j++){
        unsigned long s = 0;
        for (unsigned int i=0; i<nRows(); i++){
            s += (*this)(i, j) << (nRows() - i -1);
        }
        res[j] = s;
    }
    return res;
    
}

GeneratingMatrix::Row GeneratingMatrix::operator[](unsigned int i) const
{
    return m_data[i];
}

GeneratingMatrix::Row& GeneratingMatrix::operator[](unsigned int i)
{
    return m_data[i];
}

bool GeneratingMatrix::operator()(unsigned int i, unsigned j) const
{
    return m_data[i][j];
}

GeneratingMatrix::reference GeneratingMatrix::operator()(unsigned int i, unsigned int j)
{
    return m_data[i][j];
}

void GeneratingMatrix::flip(unsigned int i, unsigned int j)
{
    m_data[i].flip(j);
}

void GeneratingMatrix::swap_rows(unsigned int i1, unsigned int i2){
    std::swap(m_data[i1], m_data[i2]);
}

void GeneratingMatrix::swap_columns(unsigned int j1, unsigned int j2)
{
    assert(j1 < m_cols && j2 < m_cols);
    for(unsigned int i = 0; i < m_rows; ++i)
    {
        bool tmp = m_data[i][j1];
        m_data[i][j1] = m_data[i][j2];
        m_data[i][j2] = tmp;
    }
};

void GeneratingMatrix::permuteColumns(const std::vector<unsigned int>& permutation)
{
    std::vector<boost::dynamic_bitset<>> tmp(m_data);
    for(unsigned int j = 0; j < m_cols; ++j)
    {
        for(unsigned int i = 0; i < m_rows; ++i)
        {
            m_data[i][j] = tmp[i][permutation[j]];
        }
    }
}

GeneratingMatrix GeneratingMatrix::subMatrix(unsigned int nRows, unsigned int nCols) const
{
    return subMatrix(0,nRows,nCols);
}

GeneratingMatrix GeneratingMatrix::subMatrix(unsigned int startingRow, unsigned int nRows, unsigned int nCols) const
{
    GeneratingMatrix res(nRows, nCols);
    for(unsigned int i = 0; i < nRows; ++i)
    {
        auto tmp = (*this)[i+startingRow];
        tmp.resize(nCols);
        res[i] = std::move(tmp);
    }
    return res;
}

GeneratingMatrix GeneratingMatrix::subMatrix(unsigned int startingRow, unsigned int startingCol, unsigned nRows, unsigned nCols) const 
{
    GeneratingMatrix res(nRows, nCols);
    for(unsigned int i = 0; i < nRows; ++i)
    {
        for(unsigned int j = 0; j < nCols; ++j)
        {
            res(i,j) = (*this)(startingRow+i, startingCol+j);
        }
    }
    return res;
}

void GeneratingMatrix::resize(unsigned int nRows, unsigned int nCols){
    for(unsigned int i = 0; i < std::min(nRows, m_rows); ++i)
    {
        (*this)[i].resize(nCols);
    }
    for(unsigned int i = m_rows; i < nRows; ++i){
        m_data.push_back(Row(nCols));
    }
    if (nRows < m_rows)
    {
        m_data.erase(m_data.begin()+nRows, m_data.begin()+m_rows);
    }
    m_rows = nRows;
    m_cols = nCols;
}

void GeneratingMatrix::vstack(GeneratingMatrix m)
{
    m_rows += m.nRows();
    m_data.resize(m_rows);
    std::move(m.m_data.begin(),m.m_data.end(),m_data.end()-m.nRows());
}

std::ostream& operator<<(std::ostream& os, const GeneratingMatrix& mat)
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

GeneratingMatrix GeneratingMatrix::operator*(const GeneratingMatrix& m) const
{
    GeneratingMatrix res(nRows(),m.nCols());
    for(unsigned int i = 0; i < res.nRows(); ++ i)
    {
        for(unsigned int j = 0; j < m.nCols(); ++j)
        {
            unsigned int val = 0;
            for(unsigned int k = 0; k < m_rows; ++k)
            {
                val += (*this)(i,k) * m(k,j);
                val %= 2;
            }
            res(i,j) = val; 
        }
    }
    return res;
}

void GeneratingMatrix::stackRight(const GeneratingMatrix& block)
{
    resize(m_rows, m_cols+block.nCols());
    for(unsigned int i = 0; i < m_rows; ++ i)
    {
        for(unsigned int j = 0; j < block.nCols(); ++j)
        {
            (*this)(i,m_cols+j-block.nCols()) = block(i,j);
        }
    }
}

}