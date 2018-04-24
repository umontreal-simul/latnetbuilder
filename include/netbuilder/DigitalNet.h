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

#ifndef DIGITAL_NET_DIGITAL_NET_H
#define DIGITAL_NET_DIGITAL_NET_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/GeneratingMatrix.h"

namespace NetBuilder {

typedef size_t size_type;

/** Generic class to represent a digital net in any base. The integral template parameter BASE represents the base of the net. 
 * This class implements the CRTP through the template 
 * parameter DERIVED for static polymorphism purpose. */
template <typename DERIVED>
class DigitalNet {
    
   public:

    // Accessors

   /** Returns the base of the net. */
   unsigned int base() const { return derived().base(); } 

   /** Return the number of columns of generating matrices of the net. */
   unsigned int numColumns() const { return derived().numColumns(); }

   /** Returns the modulus of the net*/
   unsigned int numRows() const { return derived().numRows(); }

   /** Returns the number of points of the net */
   uInteger numPoints() const { return derived().numPoints(); }

   /** Returns the number of points of the net */
   uInteger size() const { return numPoints(); }

   /** Returns the dimension (number of coordinates) of the net.*/
   unsigned int dimension() const { return derived().dimension(); }

   /** Returns a vector containing all the generating matrices of the net. */
   std::vector<GeneratingMatrix> generatingMatrices() const {return derived().generatingMatrices(); }

   /** Returns the generating matrix of the net corresponding to the given coordinate.
    * @param coord an integer constant refering to the coordinate
   */
   GeneratingMatrix generatingMatrix(unsigned int coord) const {return derived().generatingMatrix(coord); }

    protected:
        /** Default constructor. This constructor is protected because DigitalNet is a generic class that 
         * should no be directly instantiated. */
        DigitalNet() = default;

    private:

        /** Returns a reference to the object statically cast to the DERIVED class. */
        DERIVED& derived()
        { return static_cast<DERIVED&>(*this); }

        /** Returns a constant reference to the object statically cast to the DERIVED class. */
        const DERIVED& derived() const
        { return static_cast<const DERIVED&>(*this); }
}
;

}

#endif
