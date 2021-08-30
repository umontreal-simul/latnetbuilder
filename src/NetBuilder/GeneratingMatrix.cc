// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#include <algorithm>

namespace NetBuilder {

GeneratingMatrix::GeneratingMatrix(unsigned int nRows, unsigned int nCols):
    m_data(nRows,boost::dynamic_bitset<>(nCols)),
    m_nRows(nRows),
    m_nCols(nCols)
{};

GeneratingMatrix::GeneratingMatrix(unsigned int nRows, unsigned int nCols, std::vector<uInteger> init):
    m_data(nRows),
    m_nRows(nRows),
    m_nCols(nCols)
    {
        assert(init.size() == m_nRows);
        for(unsigned int i = 0; i < m_nRows; ++i)
        {
            m_data[i] = boost::dynamic_bitset<>(nCols,init[i]);
        }
    };

unsigned int GeneratingMatrix::nCols() const { return m_nCols; }

unsigned int GeneratingMatrix::nRows() const { return m_nRows; }

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

GeneratingMatrix GeneratingMatrix::fromColsReverse(unsigned int nInputBits, unsigned int nOutputRows, std::vector<unsigned long> columns){
    GeneratingMatrix result(nInputBits, columns.size());
    for (unsigned int c=0; c<columns.size(); c++){
        unsigned long s = columns[c];
        unsigned long row = nInputBits - 1;
        while (s > 0){
            if (s % 2 == 1){
                if (row < 0){
                    throw std::runtime_error("The column in integer representation " + std::to_string(columns[c]) + "has more than " + std::to_string(nInputBits) + " bits.");
                }
                result(row, c) = 1;
            }
            s = s / 2;
            row -= 1;
        }
    }
    return result.subMatrix(0, 0, nOutputRows, columns.size());
    
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

void GeneratingMatrix::swapRows(unsigned int i1, unsigned int i2){
    std::swap(m_data[i1], m_data[i2]);
}

GeneratingMatrix GeneratingMatrix::upperLeftSubMatrix(unsigned int nRows, unsigned int nCols) const
{
    return subMatrix(0, 0, nRows, nCols);
}

GeneratingMatrix GeneratingMatrix::subMatrix(unsigned int startingRow, unsigned int startingCol, unsigned nRows, unsigned nCols) const 
{
    GeneratingMatrix res(nRows, nCols);
    for(unsigned int i = 0; i < nRows; ++i)
    {
        if (startingCol==0)
        {
            auto tmp = (*this)[i+startingRow];
            tmp.resize(nCols);
            res[i] = std::move(tmp);
        }
        else
        {
            for(unsigned int j = 0; j < nCols; ++j)
            {
                res(i,j) = (*this)(startingRow+i, startingCol+j);
            }
        }
    }
    return res;
}

void GeneratingMatrix::resize(unsigned int nRows, unsigned int nCols){
    for(unsigned int i = 0; i < std::min(nRows, m_nRows); ++i)
    {
        (*this)[i].resize(nCols);
    }
    for(unsigned int i = m_nRows; i < nRows; ++i){
        m_data.push_back(Row(nCols));
    }
    if (nRows < m_nRows)
    {
        m_data.erase(m_data.begin()+nRows, m_data.begin()+m_nRows);
    }
    m_nRows = nRows;
    m_nCols = nCols;
}

void GeneratingMatrix::stackBelow(GeneratingMatrix m)
{
    m_nRows += m.nRows();
    m_data.resize(m_nRows);
    std::move(m.m_data.begin(),m.m_data.end(),m_data.end()-m.nRows());
}

std::ostream& operator<<(std::ostream& os, const GeneratingMatrix& mat)
{
    for(unsigned int i = 0; i < mat.m_nRows; ++i)
    {
        for(unsigned int j = 0; j < mat.m_nCols; ++j)
        {
            os << mat(i,j);
            if (j < mat.m_nCols - 1)
                os << " ";
        }
        os << std::endl;
    }
    return os; 
}

std::string GeneratingMatrix::formatToColumnsReverse(unsigned int nBits) const
{
    const std::vector<unsigned long> columns =  getColsReverse();
    std::string res;
    for(unsigned int i = 0; i < (unsigned int) nCols(); ++i)
    {
        res += std::to_string(columns[i] << (nBits - nRows())) + " ";
    }
    res.pop_back();
    return res;
}

GeneratingMatrix GeneratingMatrix::operator*(const GeneratingMatrix& m) const
{
    assert ((*this).nCols() == m.nRows());
    GeneratingMatrix res(nRows(),m.nCols());

    for (unsigned int i=0; i<(*this).nRows(); i++){
        for (unsigned int j=0; j<(*this).nCols(); j++){
            if ((*this)(i, j)){
                res[i] ^= m[j];
            }
        }
    }
    return res;
}

void GeneratingMatrix::stackRight(const GeneratingMatrix& block)
{
    resize(m_nRows, m_nCols+block.nCols());
    for(unsigned int i = 0; i < m_nRows; ++ i)
    {
        for(unsigned int j = 0; j < block.nCols(); ++j)
        {
            (*this)(i,m_nCols+j-block.nCols()) = block(i,j);
        }
    }
}

}