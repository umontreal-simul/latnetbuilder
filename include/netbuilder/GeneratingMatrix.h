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

/**
 * \file
 * This file contains the definition of generating matrices in base 2
 */ 

#ifndef NETBUILDER__GENERATING_MATRIX_H
#define NETBUILDER__GENERATING_MATRIX_H

#include <boost/dynamic_bitset.hpp> 
#include <iostream>
#include <string>
#include <algorithm>


namespace NetBuilder {

/** This class implements a generating matrix of a digital net in base 2.
 *  A matrix is represented as a <code>std::vector</code> of rows implemented by
 *  <code>boost::dynamic_bitset</code>.
 */ 
class GeneratingMatrix {

    public:

        /// Type for the rows of the matrices.
        typedef boost::dynamic_bitset<> Row; 

        /// Type for references to rows.
        typedef Row::reference reference;

        /// Type for unsigned long.
        typedef unsigned long uInteger;

        /** Constructs a generating matrix with all entries set to zero.
         * @param nRows Number of rows.
         * @param nCols Number of columns.
         */  
        GeneratingMatrix(unsigned int nRows = 0, unsigned int nCols = 0);

        /** Constructs a generating matrix with rows initialized 
         * using the given integers.
         * @param nRows Number of rows.
         * @param nCols Number of columns.
         * @param init  Vector of uInteger of length nRows
         */  
        GeneratingMatrix(unsigned int nRows, unsigned int nCols, std::vector<uInteger> init);

        // GeneratingMatrix(unsigned int nRows, unsigned int nCols, bool uniTriangular);

        /** Returns the number of columns of the matrix. */
        unsigned int nCols() const;

        /** Returns the number of rows of the matrix. */
        unsigned int nRows() const;

        /** Resizes the matrix to the given shape. Potential new elements are set to zero.
         * @param nRows is the new number of rows of the matrix
         * @param nCols is the new number of columns of the matrix
         */ 

        void resize(unsigned int nRows, unsigned int nCols);
        /** Returns the element at position \c i, \c j of the matrix.
         * @param i Row index.
         * @param j Column index.
         */ 
        bool operator()(unsigned int i, unsigned j) const;

        /** Returns a reference to the element at position \c i, \c j of the matrix.
         * @param i Row index.
         * @param j Column index.
         */ 
        reference operator()(unsigned int i, unsigned int j);

        /** Flip the element at at position \c i, \c j of the matrix.
         * @param i Row index.
         * @param j Column index.
         */ 
        void flip(unsigned int i, unsigned int j);
         

        /** Returns the row at position \c i of the matrix.
         * @param i Position of the row.
         */ 
        Row operator[](unsigned int i) const;

        /** Returns a reference to the row at position \c i of the matrix.
         * @param i Position of the row.
         */ 
        Row& operator[](unsigned int i);

        /** Returns the upper-left submatrix with \c nRows rows and \c nCols columns.
         * @param nRows Number of rows.
         * @param nCols Number of columns.
         * @return A copy of the submatrix.
         */ 
        GeneratingMatrix upperLeftSubMatrix(unsigned int nRows, unsigned int nCols) const;

        /** Returns the submatrix with upper-left corner at position \c startingRow, \c startingCol
         * with \c nRows rows and \c nCols columns.
         * @param startingRow Row position of the upper-left corner.
         * @param startingCol Column position of the upper-left corner.
         * @param nRows Number of rows.
         * @param nCols Number of columns.
         * @return A copy of the submatrix.
         */ 
        GeneratingMatrix subMatrix(unsigned int startingRow, unsigned int startingCol, unsigned nRows, unsigned nCols) const;

        /**
         * Computes the product of the matrix by matrix \c m.
         * @param m Right multiplier.
         */ 
        GeneratingMatrix operator*(const GeneratingMatrix& m) const;

        /** Swap the rows at position i1 and i2 of the matrix.
         * @param i1 Position of the first row.
         * @param i2 Position of the second row.
         */
        void swapRows(unsigned int i1, unsigned int i2);

        /**
         * Extends the matrix by stacking on the right the matrix \c block.
         * @param block The matrix to stack. Should have the same number of rows as the base matrix.
         */ 
        void stackRight(const GeneratingMatrix& block);

        /**
         * Extends the matrix by stacking below the matrix \c block.
         * @param block The matrix to stack. Should have the same number of columns as the base matrix.
         */ 
        void stackBelow(GeneratingMatrix block);

        /** Overloads of << operator to print matrices. */
        friend std::ostream& operator<<(std::ostream& os, const GeneratingMatrix& mat);

        /** Returns an integer representation of the columns of the matrix. A column is read as a bit string
         * with highest bit in first position.
         */ 
        std::vector<unsigned long> getColsReverse() const;
    private:
        std::vector<boost::dynamic_bitset<>> m_data; // data internal representantion
        unsigned int m_nRows; // number of rows of the matrix
        unsigned int m_nCols; // number of columns of the matrix
};

}
#endif