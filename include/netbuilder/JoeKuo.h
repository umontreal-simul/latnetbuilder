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

#ifndef NET_BUILDER__JOE_KUO_H
#define NET_BUILDER__JOE_KUO_H

#include "latticetester/Weights.h"

#include "netbuilder/Types.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/Util.h"

namespace NetBuilder { 
/**
 * This namespace contains the various parameters used by S. Joe and F. Y. Kuo in \cite rJOE08a to find Sobol' nets
 with better two-dimensional projections.
 */
namespace JoeKuo {

/// Pair made of a coordinate and a sequence of direction numbers
typedef typename NetConstructionTraits<NetConstruction::SOBOL>::GenValue DirectionNumbers;

/// Size of generating matrices
typedef typename NetConstructionTraits<NetConstruction::SOBOL>::SizeParameter MatrixSize;

/**
 * Weights used by Joe and Kuo in \cite rJOE08a.
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
 * \f$D^{(q)} \f$Combiner used by Joe and Kuo in \cite rJOE08a.
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

/**
 * Read the raw direction numbers suggested by Joe and Kuo in \cite rJOE08a. These can be used
 * to compute generating matrices for a Sobol' net in dimension \c dimension.
 * @param dimension Dimension of the net.
 */ 
std::vector<std::vector<uInteger>> readJoeKuoDirectionNumbers(Dimension dimension);

/**
 * Get the direction numbers suggested by Joe and Kuo in \cite rJOE08a, composed by the pairs of
 * raws vector values and associated dimension of the net. These can be used
 * to compute generating matrices for a Sobol' net in dimension \c dimension.
 * @param dimension Dimension of the net.
 */ 
std::vector<DirectionNumbers> getJoeKuoDirectionNumbers(Dimension dimension);

/**
 * Constructs a Sobol' net using the direction numbers suggested by Joe and Kuo in \cite rJOE08a.
 * @param dimension Dimension of the net.
 * @param size Size of the generating matrices.
 */ 
DigitalNetConstruction<NetConstruction::SOBOL> createJoeKuoSobolNet(Dimension dimension, MatrixSize size);

/**
 * Constructs a Sobol' net using the direction numbers suggested by Joe and Kuo in \cite rJOE08a.
 * @param dimension Dimension of the net.
 * @param size Size of the generating matrices.
 * @return A <code>std::unique_ptr</code> to the constructed net.
 */ 
std::unique_ptr<DigitalNetConstruction<NetConstruction::SOBOL>> createPtrToJoeKuoSobolNet(Dimension dimension, MatrixSize size);

}}

#endif
