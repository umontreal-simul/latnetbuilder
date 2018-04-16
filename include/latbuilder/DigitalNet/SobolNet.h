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

#ifndef DIGITAL_NET_SOBOL_NET_H
#define DIGITAL_NET_SOBOL_NET_H

#include "latbuilder/Types.h"
#include "latbuilder/Util.h"
#include "latbuilder/DigitalNet/DigitalNet.h"

namespace LatBuilder { namespace DigitalNet {

/** Class which represents Sobol' net in base 2. This class inherits from DigitalNet which is a generic class 
 * to represent digital nets. DigitalNet implements the CRTP pattern and is also templated by an integral template parameter 
 * corresponding to the base.
 */

class SobolNet : public DigitalNet<SobolNet,2>{

    public:
    /** Constructs a Sobol' net in base 2. Performs sanity checks to ensure the net is not ill-formed.
     * @param modulus is the modulus of the net (number of rows and columns in the generating matrices)
     * @param dimension is the dimension of the net
     * @param directionNumbers are the initialization numbers for the recurrence accross the columns of the generating matrices. 
     */
      SobolNet(int modulus, int dimension, std::vector<std::vector<uInteger>> directionNumbers):
        m_base(2),
        m_dimension(dimension),
        m_modulus(modulus),
        m_numPoints(intPow(2,modulus)),
        m_directionNumbers(std::move(directionNumbers))
      {
        // performs a sanity check on directionNumbers
        assert(m_directionNumbers.size()==m_dimension); // assert there is a set of direction numbers by coordinate
        for(int i = 0; i < m_dimension ; ++i)
        {
          int degree = nthPrimitivePolynomialDegree(i);
          assert(m_directionNumbers[i].size() == degree); // assert each direction numbers set has the correct size
          for(int j = 0; j < degree; ++j)
          {
            assert(m_directionNumbers[i][j] % 2 == 1); // assert each direction number is odd
            assert(m_directionNumbers[i][j] < (2<<j)); // assert each direction number is small enough
          }
        } 
      };
      
      /** Returns the base of the net.  */
      unsigned int base() const { return m_base; } 

      /** Return the modulus of the net. */
      unsigned int modulus() const { return m_modulus; }

      /** Return the modulus of the net. */
      unsigned int numColumns() const { return m_modulus; }

      /** Return the modulus of the net. */
      unsigned int numRows() const { return m_modulus; }

      /** Return the number of points of the net. */
      uInteger numPoints() const { return m_numPoints; }

      /** Returns the dimension of the net */
      unsigned int dimension() const { return m_dimension; }

      /** Returns the degree of the nth primitive polynomial overs F_2.
       * @param n number of the primitive polynomial. 
       * @return 0 if n outside the accepted range.
       */ 
      static unsigned int nthPrimitivePolynomialDegree(unsigned int n){
        if (n>0)
        {
          return nthPrimitivePolynomial(n).first;
        }
        else{
          return 0;
        }
      }
 
      /** Returns a vector containing all the generating matrices of the net. */
      std::vector<GeneratingMatrix> generatingMatrices() const;

     /** Returns the generating matrix of the net corresponding to the given coordinate.
      * @param coord an integer constant refering to the coordinate
      */
      GeneratingMatrix generatingMatrix(unsigned int coord) const;

    private:
      unsigned int m_base; // base of the net
      unsigned int m_dimension; // dimension of the net
      unsigned int m_modulus; // modulus of the net 
      uInteger m_numPoints; // size of the net
      std::vector<std::vector<uInteger>> m_directionNumbers; // set of direction numbers

      typedef std::pair<unsigned int,uInteger> PrimitivePolynomial; 

      /** Returns a binary presentation of the nth primitive polynomial over F^2.
       * @param n rank of the primitie polynomial to return
       */ 
      static PrimitivePolynomial nthPrimitivePolynomial(unsigned int n); // returns a representation of the nt

      };
}}
#endif
