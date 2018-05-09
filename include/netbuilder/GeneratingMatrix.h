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

#ifndef NETBUILDER__MATRIX_H
#define NETBUILDER__MATRIX_H

#include <boost/dynamic_bitset.hpp> 
#include <iostream>
#include <string>
#include <algorithm>
// #include "netbuilder/Types.h"


namespace NetBuilder {

/** This class implements a generating matrix of a digital net in base 2.
 *  A matrix is represented as a std::vector of Rows where Row is actually
 *  a boost::dynamic_bitset.
 */ 
class GeneratingMatrix {

    public:

        typedef boost::dynamic_bitset<> Row; 
        typedef Row::reference reference;
        typedef unsigned long uInteger;

        /** Constructs a generating matrix with all entries set to zero.
         * @param n_rows is the number of rows of the matrix
         * @param n_cols is the number of columns of the matrix
         */  
        GeneratingMatrix(unsigned int n_rows, unsigned int n_cols);

        /** Constructs a generating matrix with rows initialized 
         * using the given integers.
         * @param n_rows is the number of rows of the matrix
         * @param n_cols is the number of columns of the matrix
         * @param init is a vector of uInteger of length n_rows
         */  
        GeneratingMatrix(unsigned int n_rows, unsigned int n_cols, std::vector<uInteger> init);

        /*
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
        };*/

        /** Constructs a random matrix. 
         * @param n_rows is the number of rows of the matrix
         * @param n_cols is the number of columns of the matrix 
         * @param uniTriangular is a boolean indicating whether the matrix should be unitriangular
         */ 

        GeneratingMatrix(unsigned int n_rows, unsigned int n_cols, bool uniTriangular);

        /** Returns the number of columns of the matrix. */
        unsigned int nCols() const;

        /** Returns the number of rows of the matrix. */
        unsigned int nRows() const;

        std::vector<unsigned long> getColsReverse() const;

        /** Returns the row at position \c i of the matrix
         * @param i is the position of the row
         */ 
        Row operator[](unsigned int i) const;

        /** Returns a reference to the row at position \c i of the matrix
         * @param i is the position of the row
         */ 
        Row& operator[](unsigned int i);

        /** Returns a the element at position \c i, \c j of the matrix.
         * @param i is the row index
         * @param j is the column index
         */ 
        bool operator()(unsigned int i, unsigned j) const;

        /** Returns a reference to the element at position \c i, \c j of the matrix.
         * @param i is the row index
         * @param j is the column index
         */ 
        reference operator()(unsigned int i, unsigned int j);

        /** Flip the element at at position \c i, \c j of the matrix.
         * @param i is the row index
         * @param j is the column index
         */ 
        void flip(unsigned int i, unsigned int j);

        /** Swap the rows at position i1 and i2 of the matrix.
         * @param i1 is the position of the first row
         * @param i2 is the position of the second row
         */ 
        void swap_rows(unsigned int i1, unsigned int i2);

        /** Swap the columns at position j1 and j2 of the matrix.
         * @param j1 is the position of the first row
         * @param j2 is the position of the second row
         */ 
        void swap_columns(unsigned int j1, unsigned int j2);

        void permuteColumns(const std::vector<unsigned int>& permutation);

        /** Returns the upper-left submatrix with \c nRows rows and \c nCols columns.
         * @param nRows is the number of rows of the matrix
         * @param nCols is the number of columns of the matrix
         */ 
        GeneratingMatrix subMatrix(unsigned int nRows, unsigned int nCols) const;

        GeneratingMatrix subMatrix(unsigned int startingRow, unsigned int nRows, unsigned int nCols) const;

        GeneratingMatrix subMatrix(unsigned int startingRow, unsigned int startingCol, unsigned nRows, unsigned nCols) const;

        GeneratingMatrix operator*(const GeneratingMatrix& m) const;

        void stackRight(const GeneratingMatrix& block);

        /** Resizes the matrix to the given shape. Potential new elements are set to zero.
         * @param nRows is the new number of rows of the matrix
         * @param nCols is the new number of columns of the matrix
         */ 
        void resize(unsigned int nRows, unsigned int nCols);

        void vstack(GeneratingMatrix m);

        /** Overloads of << operator the print matrices. */
        friend std::ostream& operator<<(std::ostream& os, const GeneratingMatrix& mat);

    private:
        std::vector<boost::dynamic_bitset<>> m_data; // data internal representantion
        unsigned int m_rows; // number of rows of the matrix
        unsigned int m_cols; // number of columns of the matrix
};

}
#endif