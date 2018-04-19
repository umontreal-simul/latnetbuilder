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

#ifndef DIGITAL_NET_EXPLICIT_NET_H
#define DIGITAL_NET_EXPLICIT_NET_H

#include "latbuilder/Types.h"
#include "latbuilder/Util.h"
#include "latbuilder/DigitalNet/DigitalNet.h"

namespace LatBuilder { namespace DigitalNet {

/** Class which represents digital net in any base. This class inherits from DigitalNet which is a generic class 
 * to represent digital nets. DigitalNet implements the CRTP pattern and is also templated by an integral template parameter 
 * corresponding to the base. ExplicitNet enables to use any generating matrices to define the digital net.
 */

template <uInteger BASE>
class ExplicitNet : public DigitalNet<ExplicitNet<BASE>,BASE>{

    public:

    typedef typename DigitalNet<ExplicitNet<BASE>,BASE>::GeneratingMatrix GeneratingMatrix;

    /** Constructs a digital net in any base from arbitrary generating matrices. Perfoms a sanity check to ensure the matrices
     * have the right size. Other parameters are deduced from the given generating matrices.
     * @param generatingMatrices is the vector of generating matrices
     */
    ExplicitNet(std::vector<GeneratingMatrix> generatingMatrices):
    m_generatingMatrices(generatingMatrices),
    m_base(BASE)
    {
      m_dimension = m_generatingMatrices.size();
      m_rows = m_generatingMatrices[0].nRows();
      m_cols = m_generatingMatrices[0].nCols();
      m_numPoints = intPow(m_base,m_cols);
      for(const auto& mat : m_generatingMatrices)
      {
        assert(mat.nRows()==m_rows);
        assert(mat.nCols()==m_cols);
      }
    };
      
      /** Returns the base of the net.  */
      unsigned int base() const { return m_base; } 

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
      unsigned int m_base; // base of the net
      unsigned int m_dimension; // dimension of the net
      uInteger m_numPoints; // size of the net
      unsigned int m_rows; // number of rows of generating matrices 
      unsigned int m_cols; // number of columns of generating matrices
      std::vector<GeneratingMatrix> m_generatingMatrices; // set of direction numbers

      };
}}
#endif
