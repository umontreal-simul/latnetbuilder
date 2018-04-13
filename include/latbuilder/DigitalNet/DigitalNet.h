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
#include "latbuilder/Types.h"
#include "latbuilder/Util.h"

namespace LatBuilder { namespace DigitalNet {

typedef std::vector<std::vector<short>> GeneratingMatrix;

typedef size_t size_type;

template <typename DERIVED, uInteger BASE>
class DigitalNet {
    
   public:
   // returns the base of the digital net
   uInteger base() const { return derived().base(); } 

   // returns the modulus of the digital net
   size_type numColumns() const { return derived().numColumns(); }

   // returns the modulus of the digital net
   size_type numRows() const { return derived().numRows(); }

   // returns the number of points of the digital net
   size_type numPoints() const { return derived().numPoints(); }

   // returns the number of points of the digital net
   size_type size() const { return numPoints(); }

   // returns the dimension of the digital net
   size_type dimension() const { return derived().dimension(); }

   std::vector<GeneratingMatrix> generatingMatrices() const {return derived().generatingMatrices(); }

   GeneratingMatrix generatingMatrix(size_type dim) const {return derived().generatingMatrix(dim); }

    protected:
        DigitalNet() = default;

    private:

        DERIVED& derived()
        { return static_cast<DERIVED&>(*this); }

        const DERIVED& derived() const
        { return static_cast<const DERIVED&>(*this); }
}
;

template <typename DERIVED>
std::vector<std::vector<uInteger>> rolledGeneratingMatrices(const DigitalNet<DERIVED,2>& net)
{
    std::vector<std::vector<uInteger>> res;
    std::vector<GeneratingMatrix> matrices = net.generatingMatrices();
    auto m = net.numColumns();
    auto s = net.dimension();
    uInteger acc;

    for (int i = 0; i<s; ++i)
    {
        std::vector<uInteger> tmp;
        for(int j = 0; j < net.numRows(); ++j)
        {
            acc = 0;
            for(int k = 0; k<m; ++k){
                acc += matrices[i][j][k] << (m-k-1);
            }
            tmp.push_back(acc);
        }
        res.push_back(tmp);
    }
    return res;
}

}}

#endif
