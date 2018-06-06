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

#ifndef LATBUILDER__SIZE_PARAM_H
#define LATBUILDER__SIZE_PARAM_H

#include "latbuilder/Types.h"
#include "latbuilder/Util.h"

#include <ostream>

namespace LatBuilder {

/**
 * Lattice size parameter.
 */
template <LatticeType, PointSetType> class SizeParam;

/**
 * SizeParam traits.
 *
 * Specializations of this class template must define the type:
 * - Modulus: type of the modulus;
 * - size_type : type for the number of points in the lattice;
 * And must define the static function:
 * - size_type ComputeNumPoints(const Modulus& modulus) number of the points in the lattice point set 
 */
template <class DERIVED>
struct SizeParamTraits;

/**
 * SizeParam traits for ordinary lattice rule.
 *
 */
template<PointSetType PST>
struct SizeParamTraits<SizeParam<LatticeType::ORDINARY,PST>>{
  ///  type for modulus (= number of points) values.
  typedef typename LatticeTraits<LatticeType::ORDINARY>::Modulus Modulus;
  typedef uInteger size_type;

  static size_type ComputeNumPoints(const Modulus& modulus){
    return LatticeTraits<LatticeType::ORDINARY>::NumPoints(modulus);
  }
  
};

/**
 * SizeParam traits for digital lattice rule.
 *
 */
template<PointSetType PST>
struct SizeParamTraits<SizeParam<LatticeType::DIGITAL,PST>>{
  ///  type for modulus (= number of points) values.
  typedef typename LatticeTraits<LatticeType::DIGITAL>::Modulus Modulus;
  typedef uInteger size_type;

  static size_type ComputeNumPoints(const Modulus& modulus){
    return LatticeTraits<LatticeType::DIGITAL>::NumPoints(modulus);
  }
  
};
/**
 * SizeParam traits for polynomial lattice rule.
 *
 */
template<PointSetType PST>
struct SizeParamTraits<SizeParam<LatticeType::POLYNOMIAL,PST>>{
  ///  type for modulus values.
  typedef typename LatticeTraits<LatticeType::POLYNOMIAL>::Modulus Modulus;
  typedef uInteger size_type;

  static size_type ComputeNumPoints(const Modulus& modulus){
    return LatticeTraits<LatticeType::POLYNOMIAL>::NumPoints(modulus);
  }
  
};

/**
 * Abstract CRTP base class for lattice size parameter.
 */
template <class DERIVED>
class BasicSizeParam {
protected:
   BasicSizeParam(const BasicSizeParam&) = default;

public:

   typedef typename SizeParamTraits<DERIVED>::Modulus Modulus;
   typedef typename SizeParamTraits<DERIVED>::size_type size_type;

   BasicSizeParam(Modulus modulus): m_modulus(modulus),
                                    m_numPoints(SizeParamTraits<DERIVED>::ComputeNumPoints(m_modulus)) { }

   Modulus modulus() const { return m_modulus; }
   size_type numPoints() const { return m_numPoints; }
   operator Modulus() const { return m_modulus; }

   /**
    * Returns the value of Euler's totient function.
    * -For ordinary lattices: It is the number of positive integers that are smaller than and coprime
    * to the modulus.
    * -For polynomial lattices: It is the number of polynomials coprime with the modulus and with (strictly) smaller degree
    */
   size_t totient() const
   { return derived().totient(); }

   template <class D> bool operator== (const BasicSizeParam<D>& other) const { return modulus() == other.modulus(); }
   template <class D> bool operator!= (const BasicSizeParam<D>& other) const { return !operator==(other); }
   template <class D> bool operator<  (const BasicSizeParam<D>& other) const { return m_modulus < other.m_modulus; }

   /**
    * Divides the merit value \c merit by the number of points.
    */
   void normalize(Real& merit) const
   { derived().normalize(merit); }

   /**
    * Divides the each merit value in \c merit by the number of points.
    */
   void normalize(RealVector& merit) const
   { derived().normalize(merit); }

   unsigned int log2NumPoints() const
   { 
     return (unsigned int) log2Int( (unsigned int) numPoints() );
   }

private:
   Modulus m_modulus;
   size_type m_numPoints;

   template <class D>
   friend std::ostream& operator<<(std::ostream&, const BasicSizeParam<D>&);

   std::ostream& format(std::ostream& os) const
   { return derived().format(os); }

   DERIVED& derived()
   { return static_cast<DERIVED&>(*this); }

   const DERIVED& derived() const
   { return static_cast<const DERIVED&>(*this); }

};

template <class D>
std::ostream& operator<<(std::ostream& os, const BasicSizeParam<D>& sizeParam)
{ return sizeParam.format(os); }

}

#include "latbuilder/SizeParam-SIMPLE.h"
#include "latbuilder/SizeParam-EMBEDDED.h"

#endif
