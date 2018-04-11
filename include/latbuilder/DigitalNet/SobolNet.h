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

typedef uInteger Modulus;

typedef std::pair<uInteger,uInteger> PrimitivePolynomial;

class SobolNet : public DigitalNet<SobolNet,2>{

    public:
      SobolNet(Modulus modulus, uInteger dimension, std::vector<std::vector<uInteger>> directionNumbers):
        m_base(2),
        m_dimension(dimension),
        m_modulus(modulus),
        m_numPoints(intPow(2,modulus)),
        m_directionNumbers(std::move(directionNumbers))
      {};
      
      uInteger base() const { return m_base; } 

      // returns the modulus of the digital net
      size_type modulus() const { return m_modulus; }

      // returns the modulus of the digital net
      size_type numColumns() const { return m_modulus; }

      // returns the modulus of the digital net
      size_type numRows() const { return m_modulus; }

      // returns the number of points of the digital net
      size_type numPoints() const { return m_numPoints; }

      // returns the dimension of the digital net
      size_type dimension() const { return m_dimension; }

      // TO IMPLEMENT
      static PrimitivePolynomial nthPrimitivePolynomial(uInteger n);

      static uInteger nthPrimitivePolynomialDegree(uInteger n){
        return nthPrimitivePolynomial(n).first;
      }

      // TO IMPLEMENT   
      std::vector<GeneratingMatrix> generatingMatrices() const;

      // TO IMPLEMENT
      static GeneratingMatrix generatingMatrix(uInteger coord, Modulus m, std::vector<uInteger> directionNumbers);

    private:
      uInteger m_base;
      size_type m_dimension;
      size_type m_modulus;
      size_type m_numPoints;
      std::vector<std::vector<uInteger>> m_directionNumbers;  
      };
}}
#endif
