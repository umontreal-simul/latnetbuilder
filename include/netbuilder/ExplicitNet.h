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

#ifndef NET_BUILDER__EXPLICIT_NET_H
#define NET_BUILDER__EXPLICIT_NET_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"

namespace NetBuilder {

/** Class which represents digital net in any base. This class inherits from DigitalNet which is a generic class 
 * to represent digital nets. DigitalNet implements the CRTP pattern and is also templated by an integral template parameter 
 * corresponding to the base. ExplicitNet enables to use any generating matrices to define the digital net.
 */

class ExplicitNet : public DigitalNet<ExplicitNet>{

    public:

    ExplicitNet(unsigned int nRows, unsigned int nCols):
    m_dimension(0),
    m_rows(nRows),
    m_cols(nCols)
    {};

    /** Constructs a digital net in any base from arbitrary generating matrices. Perfoms a sanity check to ensure the matrices
     * have the right size. Other parameters are deduced from the given generating matrices.
     * @param generatingMatrices is the vector of generating matrices
     */    
    ExplicitNet(std::vector<GeneratingMatrix> generatingMatrices):
    m_generatingMatrices(generatingMatrices)
    {
      m_dimension = m_generatingMatrices.size();
      m_rows = m_generatingMatrices[0].nRows();
      m_cols = m_generatingMatrices[0].nCols();
      m_numPoints = intPow(2,m_cols);
      for(const auto& mat : m_generatingMatrices)
      {
        assert(mat.nRows()==m_rows);
        assert(mat.nCols()==m_cols);
      }
    };

    template <typename RAND>
    ExplicitNet(unsigned int dimension, unsigned int nRows, unsigned int nCols, RAND& randomGen):
    m_dimension(dimension),
    m_rows(nRows),
    m_cols(nCols),
    m_generatingMatrices(dimension,GeneratingMatrix(m_rows,m_cols))
    {
      for (unsigned int dim = 0; dim < m_dimension; ++dim)
      {
        m_generatingMatrices[dim] = GeneratingMatrix::randomMatrix(m_rows,m_cols,randomGen);
      }
    };

    void extendDimension(GeneratingMatrix genMatrix)
    {
      assert(genMatrix.nRows() == m_rows);
      assert(genMatrix.nCols() == m_cols);
      ++m_dimension;
      m_generatingMatrices.push_back(genMatrix);
    };

    void shrinkDimension()
    {
      --m_dimension;
      m_generatingMatrices.pop_back();
    };
      

      /** Return the number of columns of the generating matrices of the net. */
      unsigned int numColumns() const { return m_cols; }

      /** Return the modulus of the generating matrices of the net. */
      unsigned int numRows() const { return m_rows; }

      /** Return the number of points of the net. */
      uInteger numPoints() const { return m_numPoints; }

      /** Returns the dimension of the net */
      unsigned int dimension() const { return m_dimension; }
 
      /** Returns a vector containing all the generating matrices of the net. */
      std::vector<GeneratingMatrix> generatingMatrices() const
      {
        return m_generatingMatrices;
      }

     /** Returns the generating matrix of the net corresponding to the given coordinate.
      * @param coord is an integer refering to the coordinate
      */
      GeneratingMatrix generatingMatrix(unsigned int coord) const
      {
        return m_generatingMatrices[coord-1]; 
      }

    private:
      unsigned int m_dimension; // dimension of the net
      uInteger m_numPoints; // size of the net
      unsigned int m_rows; // number of rows of generating matrices 
      unsigned int m_cols; // number of columns of generating matrices
      std::vector<GeneratingMatrix> m_generatingMatrices; // set of direction numbers

      };
}
#endif
