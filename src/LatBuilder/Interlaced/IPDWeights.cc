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

#include "latbuilder/Interlaced/IPDWeights.h"
#include "netbuilder/Types.h"

#include "latticetester/CoordinateSets.h"

namespace LatBuilder { namespace Interlaced
{

template <class KERNEL>
IPDWeights<KERNEL>::IPDWeights(std::unique_ptr<LatticeTester::ProjectionDependentWeights> weights, const KERNEL& kernel):
m_interlacingFactor(kernel.interlacingFactor()),
m_kernelName(kernel.name()),
m_baseWeights(std::move(weights))
{
    LatticeTester::PODWeights fakeWeights;
    fakeWeights.getOrderDependentWeights().setDefaultWeight(1);
    fakeWeights.getProductWeights().setDefaultWeight(1);
    kernel.correctPODWeights(fakeWeights);
    typename KERNEL::CorrectionProductWeights correctionProductWeights(kernel); 
    Dimension dim(m_baseWeights->getSize());
    if (dim > 0){
        LatticeTester::CoordinateSets::FromRanges projections(0, dim * kernel.interlacingFactor(), 0, dim * kernel.interlacingFactor() - 1);
        for (auto cit = projections.begin (); cit != projections.end (); ++cit) {
            const LatticeTester::Coordinates& projection = *cit;
            LatticeTester::Coordinates realProjection;
            for(Dimension coord : projection)
            {
                realProjection.insert( coord / kernel.interlacingFactor() );
            }
            double baseWeight = m_baseWeights->getWeight(realProjection);
            if (baseWeight > 0){
                LatticeTester::ProjectionDependentWeights::setWeight(projection, baseWeight * fakeWeights.getWeight(realProjection) * correctionProductWeights.getWeight(projection));
            }
        }
    }

    for(size_t coord = 0; coord < m_baseWeights->getSize(); ++coord)
    {
        for(const auto& projWeight : m_baseWeights->getWeightsForLargestIndex(coord) )
        {
            m_baseWeights->setWeight(projWeight.first, projWeight.second * fakeWeights.getWeight(projWeight.first));
        }
    }
}

template <class KERNEL>
void IPDWeights<KERNEL>::setWeight (const LatticeTester::Coordinates & projection, double weight){
    throw runtime_error("IPD weights cannot be directly set.");
}

template <class KERNEL>
const LatticeTester::ProjectionDependentWeights& IPDWeights<KERNEL>::getBaseWeights () const {
    return *m_baseWeights;
}

template <class KERNEL>
unsigned int IPDWeights<KERNEL>::interlacingFactor() const {
    return m_interlacingFactor;
}



template <class KERNEL>
void IPDWeights<KERNEL>::format(std::ostream& os) const
{
    os << "IPDWeights:" << std::endl; 
    os << "    Kernel: " << m_kernelName << "," << std::endl;
    os << "    Base Weights: " << static_cast<LatticeTester::ProjectionDependentWeights>(*this);
}

template class IPDWeights<LatBuilder::Kernel::IAAlpha>;
template class IPDWeights<LatBuilder::Kernel::IB>;

}}
