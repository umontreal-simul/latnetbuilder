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

#ifndef NET_BUILDER__INTERLACED__IPOD_WEIGHTS_H
#define NET_BUILDER__INTERLACED__IPOD_WEIGHTS_H

#include "latbuilder/Kernel/IAAlpha.h"
#include "latbuilder/Kernel/IB.h"

#include "latticetester/Coordinates.h"
#include "latticetester/Weights.h"
#include "latticetester/ProductWeights.h"
#include "latticetester/OrderDependentWeights.h"
#include "latticetester/PODWeights.h"


#include <memory>
#include <cmath>
namespace NetBuilder { namespace Interlaced
{

using LatticeTester::Coordinates;

/**
 * Class to represent interlaced projection-dependent weights.
 * Uses operator \f$w\f$ defined in Corollary 1. of \cite rGOD13a to transform weights in dimension \f$s\f$ into weights
 * in dimension \f$ds\f$. 
 * If \f$ \mathfrak u \subseteq \{1, \dots, ds\}\f$:
 * \f[
 *  w(\mathfrak u) = \{ \lceil j/d \rceil, j \in \mathfrak u\}.
 * \f]
 * Additionally, depending on the kernel, corrections must be made. This corresponds to the correction product weights \f$\delta\f$
 * and the correction order-dependent weight \f$\Gamma\f$. The kernel implements methods to compute these correction weights.
 * To wrap it up, the formula for the interlaced weights \f$\bar{\gamma}\f$ is as follows:
 * \f[
 * \bar{\gamma}_{\mathfrak u} = \gamma_{w(\mathfrak u)} \prod_{j \in \mathfrak u} \delta_j \Gamma_{|w(\mathfrak u)|}.
 * \f]
 * for \f$ \mathfrak u \subseteq \{1, \dots, d s\} \f$.
 * @tparam KERNEL Kernel.
 */ 
template <typename KERNEL>
class IPODWeights:
    public LatticeTester::Weights
{
    public:

        /**
         * Constructor.
         * @param weights Base weights.
         * @param kernel Kernel.
         */ 
        IPODWeights(std::unique_ptr<LatticeTester::ProductWeights> weights, const KERNEL& kernel);

        /**
         * Constructor.
         * @param weights Base weights.
         * @param kernel Kernel.
         */ 
        IPODWeights(std::unique_ptr<LatticeTester::OrderDependentWeights> weights, const KERNEL& kernel);

        /**
         * Constructor.
         * @param weights Base weights.
         * @param kernel Kernel.
         */ 
        IPODWeights(std::unique_ptr<LatticeTester::PODWeights> weights, const KERNEL& kernel);

        /**
         * Returns the weight of the projection specified by `projection`.
         */
        double getWeight (const Coordinates & projection) const override;

        /**
         * Returns the interlacing factor.
         */ 
        virtual unsigned int interlacingFactor() const override;

        /**
         * Returns the correction product weight \f$\delta_j\f$.
         * @param coordinate Value of \f$j\f$.
         */ 
        double getCorrectionProductWeightForCoordinate (Coordinates::size_type coordinate) const;

        /**
         * Returns a const reference to the corrected base weights.
         * More precisely these weights equal: 
         * \f[
         *  \{1, \dots, s \} \supseteq \mathfrak u \mapsto gamma_{\mathfrak u} \Gamma_{|\mathfrak u|}
         * \f]
         */ 
        const LatticeTester::PODWeights& getBaseWeights() const;

        double getWeightForOrder (Coordinates::size_type order) const;

        double getWeightForCoordinate (Coordinates::size_type coordinate) const;

    protected:
        /**
         * Identifies the type of weights, formats them and outputs them on \c os.
         *
         * \remark Deriving classes should identify themselves in the output.
         */
        void format(std::ostream& os) const override;

    private:
        std::unique_ptr<LatticeTester::PODWeights> m_baseWeights;
        typename KERNEL::CorrectionProductWeights m_correctionProductWeights;
        unsigned int m_interlacingFactor;
        std::string m_kernelName;
};

extern template class IPODWeights<LatBuilder::Kernel::IAAlpha>;
extern template class IPODWeights<LatBuilder::Kernel::IB>;

}}

#endif