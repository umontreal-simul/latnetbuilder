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

#ifndef DIGITAL_NET_POLYNOMIAL_LATTICE_BASE2_H
#define DIGITAL_NET_POLYNOMIAL_LATTICE_BASE2_H
#include "latbuilder/DigitalNet/DigitalNetBase2.h"
#include "latbuilder/Types.h"
#include "latbuilder/Util.h"


namespace LatBuilder { namespace DigitalNet {


/**
 * Definition of a polynomial lattice in base 2 as a digital net
 *
 * 
 */
class PolynomialLatticeBase2 : public DigitalNetBase2 {
public:
   typedef typename LatBuilder::LatticeTraits<LatBuilder::LatticeType::POLYNOMIAL>::Modulus Modulus;
   typedef typename LatBuilder::LatticeTraits<LatBuilder::LatticeType::POLYNOMIAL>::GeneratingVector GeneratingVector;

   /**
    * Constructor.
    * \param modulus  modulus of the lattice.
    * \param gen vector  generating vector of the lattice.
    *
    */
   PolynomialLatticeBase2(Modulus modulus, GeneratingVector gen) :
  
   DigitalNetBase2(deg(modulus), deg(modulus), computeGeneratorMatrices(modulus,gen) ),
   m_modulus(modulus),
   m_gen(std::move(gen))
      
   {
  
    }

   // returns the modulus of the lattice
   Modulus modulus() const { return m_modulus; }

   

private:
   
   Modulus m_modulus;
   GeneratingVector m_gen;

protected :
   /**
    * computes the series expansion \f$ w(z) = \frac{h(z)}{P(z)} = \sum_{l=1}^{\infty} w_lz^{-l} \f$, where \f$\deg(h(z))<\deg(P(z)) = m\f$
    * the expansion is computed till the expansion_limit parameter.
    * expanding the coeifficients we get for \f$ l \geq 1\f$ :
    * \f[
    * w_l = - h_{m-l} + \sum_{i = \max(1,l-m)}^{l-1} w_i P_{m-(l-i)} \f]
    *
    *
    */
   void expandSeries(const Modulus& h, const Modulus& P, std::vector<uInteger>& expansion, const uInteger& expansion_limit){
      int m = (int) deg(P); 
      for(int l = 1; l<= expansion_limit ; l++){
          int res =  (m-l >=0 && IsOne(coeff(h, m-l)))? 1 : 0;
          int start = (l-m > 1) ? (l-m) : 1;
          for( int p = start; p < l; p++){
              res = ( res + expansion[p-1] * IsOne(coeff(P, m-(l-p)))) %2;        
          }
          expansion[l-1] = res;
      }
   }
   /**
    * computes the generator matrices given the modulus and the generator vector.
    * reference : \cite rDIC10a , page 299
    * 
    *
    */

   std::vector<std::vector<uInteger>> computeGeneratorMatrices(const Modulus& modulus, const GeneratingVector& gen) {
      std::vector<std::vector<uInteger>> genratorMatrices(gen.size());
      
      uInteger m = (uInteger) (deg(modulus));

      for(uInteger j=0; j<gen.size(); j++){
        
        std::vector<uInteger> expansion(2 * m);
        expandSeries(gen[j], modulus, expansion, 2 * m);

        for(uInteger c =0; c<m; c++ ){
          uInteger res = 0;
            uInteger mult = 1;
            for(uInteger row =0; row <m; row++ ){
                res += expansion[c + row] * mult;
                mult *= 2;
            }
            genratorMatrices[j].push_back(res);
        }
      }
      return genratorMatrices;
   }


};



}}
#endif
