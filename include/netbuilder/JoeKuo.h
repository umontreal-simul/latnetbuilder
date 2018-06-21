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

#ifndef NET_BUILDER__JOE_KUO_JOE_KUO_H
#define NET_BUILDER__JOE_KUO_JOE_KUO_H

#include "latticetester/Weights.h"

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"

namespace NetBuilder { 
/**
 * This namespace contains the various parameters used by S. Joe and F. Y. Kuo in \cite rJOE08a to find Sobol' nets
 with better two-dimensional projections.
 */
namespace JoeKuo {

/**
 * Weights used by Joe and Kuo in \cite rJOE08.
 */ 
class Weights : public LatticeTester::Weights {

public:

    /**
    * Destructor.
    */
    virtual ~Weights()  {} 

    virtual std::string name() const { return "Joe-Kuo weights"; }

    /**
    * Copy constructor.
    */
    Weights (const Weights &) = default;

    /**
    * Returns the weight of the projection specified by \c projection.
    */
    virtual LatticeTester::Weight getWeight (const LatticeTester::Coordinates & projection) const;

    protected:
    /// \copydoc LatticeTester::Weights::format()
    virtual void format(std::ostream& os) const;
};

/**
 * \f$D^{(q)} \f$Combiner used by Joe and Kuo in \cite rJOE08.
 * \tparam q Power used in the combiner.
 */ 
template<unsigned int q>
struct Combiner
{
    /** 
     * Combines the merits.
     */ 
    Real operator()(const RealVector& merits)
    {
        Real res = 0;
        unsigned int level = 0;
        for(Real merit : merits)
        {
            ++level;
            res = std::max(res, intPow(merit,q)/(level-merit+1));
        }
        return res;
    }
};

}}

#endif
