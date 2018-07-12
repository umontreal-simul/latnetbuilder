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

#include "netbuilder/Interlaced/IPDWeights.h"
#include "netbuilder/Types.h"

#include "latticetester/CoordinateSets.h"

namespace NetBuilder { namespace Interlaced
{

template <class KERNEL>
IPDWeights<KERNEL>::IPDWeights(std::unique_ptr<LatticeTester::ProjectionDependentWeights> weights, const KERNEL& kernel):
m_KernelName(kernel.name())
{
    LatticeTester::PODWeights fakeWeights;
    fakeWeights.getOrderDependentWeights().setDefaultWeight(1);
    fakeWeights.getProductWeights().setDefaultWeight(1);
    kernel.correctPODWeights(fakeWeights);
    typename KERNEL::CorrectionProductWeights correctionProductWeights(kernel); 
    Dimension dim(weights->getSize());
    if (dim > 0){
        LatticeTester::CoordinateSets::FromRanges projections(0, dim * kernel.interlacingFactor(), 0, dim * kernel.interlacingFactor() - 1);
        for (auto cit = projections.begin (); cit != projections.end (); ++cit) {
            const LatticeTester::Coordinates& projection = *cit;
            LatticeTester::Coordinates realProjection;
            for(Dimension coord : projection)
            {
                realProjection.insert( coord / kernel.interlacingFactor() );
            }
            double baseWeight = weights->getWeight(realProjection);
            if (baseWeight > 0){
                LatticeTester::ProjectionDependentWeights::setWeight(projection, baseWeight * fakeWeights.getWeight(realProjection) * correctionProductWeights.getWeight(projection));
            }
        }
    }
}

template <class KERNEL>
void IPDWeights<KERNEL>::setWeight (const LatticeTester::Coordinates & projection, double weight){
    throw runtime_error("IPD weights cannot be directly set.");
}


template <class KERNEL>
void IPDWeights<KERNEL>::format(std::ostream& os) const
{
    os << "IPDWeights(" << m_KernelName << ", " << static_cast<LatticeTester::ProjectionDependentWeights>(*this) << ")"; // TODO
}

template class IPDWeights<LatBuilder::Kernel::AIDNAlpha>;
template class IPDWeights<LatBuilder::Kernel::BIDN>;

}}
