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

#include "netbuilder/Interlaced/IPODWeights.h"

#include "netbuilder/Util.h"

namespace NetBuilder { namespace Interlaced
{

using LatticeTester::Coordinates;

template<typename KERNEL> IPODWeights<KERNEL>:: IPODWeights(std::unique_ptr<LatticeTester::ProductWeights> weights, const KERNEL& kernel):
m_baseWeights(std::make_unique<LatticeTester::PODWeights>()),
m_correctionProductWeights(kernel),
m_kernelName(kernel.name())
{
    m_baseWeights->getOrderDependentWeights().setDefaultWeight(1);
    m_baseWeights->getProductWeights() = *weights;
    kernel.correctPODWeights(*m_baseWeights);
    m_interlacingFactor = kernel.interlacingFactor();
}

template<typename KERNEL> IPODWeights<KERNEL>:: IPODWeights(std::unique_ptr<LatticeTester::OrderDependentWeights> weights, const KERNEL& kernel):
m_baseWeights(std::make_unique<LatticeTester::PODWeights>()),
m_correctionProductWeights(kernel),
m_kernelName(kernel.name())
{
    m_baseWeights->getOrderDependentWeights() = *weights;
    m_baseWeights->getProductWeights().setDefaultWeight(1);
    kernel.correctPODWeights(*m_baseWeights);
    m_interlacingFactor = kernel.interlacingFactor();
}

template<typename KERNEL> IPODWeights<KERNEL>:: IPODWeights(std::unique_ptr<LatticeTester::PODWeights> weights, const KERNEL& kernel):
m_baseWeights(std::make_unique<LatticeTester::PODWeights>()),
m_correctionProductWeights(kernel),
m_kernelName(kernel.name())
{
    m_baseWeights->getOrderDependentWeights() = weights->getOrderDependentWeights();
    m_baseWeights->getProductWeights() = weights->getProductWeights();
    kernel.correctPODWeights(*m_baseWeights);
    m_interlacingFactor = kernel.interlacingFactor();
}

template<typename KERNEL> double IPODWeights<KERNEL>::getWeight (const Coordinates & projection) const
{
    Coordinates realProjection;
    for(Dimension coord : projection)
    {
        realProjection.insert( coord / m_interlacingFactor );
    }
    return m_baseWeights->getWeight(realProjection) * m_correctionProductWeights.getWeight(projection);
}

template<typename KERNEL> 
double IPODWeights<KERNEL>::getWeightForOrder (Coordinates::size_type order) const
{
    return m_baseWeights->getOrderDependentWeights().getWeightForOrder(order);
}

template<typename KERNEL> 
double IPODWeights<KERNEL>::getWeightForCoordinate (Coordinates::size_type coordinate) const
{
    return m_baseWeights->getProductWeights().getWeightForCoordinate(coordinate);
}

template<typename KERNEL> unsigned int IPODWeights<KERNEL>::interlacingFactor() const
{
    return m_interlacingFactor;
}

template<typename KERNEL>
const LatticeTester::PODWeights& IPODWeights<KERNEL>::getBaseWeights() const
{
    return *m_baseWeights;
}


template<typename KERNEL> double IPODWeights<KERNEL>::getCorrectionProductWeightForCoordinate (Coordinates::size_type coordinate) const
{
    return m_correctionProductWeights.getWeightForCoordinate(coordinate);
}

template<typename KERNEL> void IPODWeights<KERNEL>::format(std::ostream& os) const
{
    os << "IPODWeights(" << m_kernelName << ", " << *m_baseWeights << ")";
}


template class IPODWeights<LatBuilder::Kernel::AIDNAlpha>;
template class IPODWeights<LatBuilder::Kernel::BIDN>;
}}
