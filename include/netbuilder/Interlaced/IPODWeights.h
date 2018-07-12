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

#include "latbuilder/Kernel/AIDNAlpha.h"
#include "latbuilder/Kernel/BIDN.h"

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

template <typename KERNEL>
class IPODWeights:
    public LatticeTester::PODWeights
{
    public:

        IPODWeights(std::unique_ptr<LatticeTester::ProductWeights> weights, const KERNEL& kernel);

        IPODWeights(std::unique_ptr<LatticeTester::OrderDependentWeights> weights, const KERNEL& kernel);

        IPODWeights(std::unique_ptr<LatticeTester::PODWeights> weights, const KERNEL& kernel);

        /**
         * Returns the double of the projection specified by `projection`.
         */
        virtual double getWeight (const Coordinates & projection) const override;

        unsigned int getInterlacingFactor() const;

        double getCorrectionProductWeightForCoordinate (Coordinates::size_type coordinate) const;

    protected:
        /**
         * Identifies the type of weights, formats them and outputs them on \c os.
         *
         * \remark Deriving classes should identify themselves in the output.
         */
        virtual void format(std::ostream& os) const override;

    private:
        typename KERNEL::CorrectionProductWeights m_correctionProductWeights;
        unsigned int m_interlacingFactor;
};

extern template class IPODWeights<LatBuilder::Kernel::AIDNAlpha>;
extern template class IPODWeights<LatBuilder::Kernel::BIDN>;

// class IPODWeightsA: public IPODWeights<LatBuilder::Kernel::AIDNAlpha>{};
}}

#endif