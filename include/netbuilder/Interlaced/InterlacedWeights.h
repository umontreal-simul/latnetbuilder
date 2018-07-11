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

#ifndef NET_BUILDER__INTERLACED__INTERLACED_WEIGHTS
#define NET_BUILDER__INTERLACED__INTERLACED_WEIGHTS

#include "latbuilder/Kernel/AIDNAlpha.h"
#include "latbuilder/Kernel/BIDN.h"

#include "latticetester/Coordinates.h"
#include "latticetester/Weights.h"
#include "latticetester/Util.h"

#include "netbuilder/Util.h"

#include <memory>
#include <cmath>
namespace NetBuilder { namespace Interlaced
{

using LatticeTester::Weights;
using LatticeTester::Coordinates;

template <class KERNEL>
class InterlacedWeights: public Weights
 {};

template <>
class InterlacedWeights<LatBuilder::Kernel::AIDNAlpha>: public Weights
{
    public:

        InterlacedWeights(std::unique_ptr<Weights> weights, const LatBuilder::Kernel::AIDNAlpha& kernel):
            m_baseWeights(std::move(weights)),
            m_interlacingFactor(kernel.interlacingFactor()),
            m_alpha(kernel.alpha()),
            m_correctionFactor(sqrt((double) (1 << (m_alpha * (2 * m_interlacingFactor - 1)))))
        {}

        /**
         * Returns the double of the projection specified by `projection`.
         */
        virtual double getWeight (const Coordinates & projection) const override
        {
            Coordinates realProjection;
            for(Dimension coord : projection)
            {
                realProjection.insert( coord / m_interlacingFactor );
            }
            return intPow(m_correctionFactor, projection.size()) * m_baseWeights->getWeight(realProjection);
        }

    protected:
        /**
         * Identifies the type of weights, formats them and outputs them on \c os.
         *
         * \remark Deriving classes should identify themselves in the output.
         */
        virtual void format(std::ostream& os) const override
        {
            os << "InterlacedWeightsA(" << m_interlacingFactor << ", " << *m_baseWeights << ")";
        }

    private:
        std::unique_ptr<Weights> m_baseWeights;
        unsigned int m_interlacingFactor;
        unsigned int m_alpha;
        double m_correctionFactor;
};

template<>
class InterlacedWeights<LatBuilder::Kernel::BIDN>:
    public Weights
{
    public:

        InterlacedWeights(std::unique_ptr<Weights> weights, const LatBuilder::Kernel::BIDN& kernel):
            m_baseWeights(std::move(weights)),
            m_interlacingFactor(kernel.interlacingFactor())
        {}

        /**
         * Returns the double of the projection specified by `projection`.
         */
        virtual double getWeight (const Coordinates & projection) const override
        {
            Coordinates realProjection;
            double correctionFactor = 1;
            for(Dimension coord : projection)
            {
                correctionFactor *= 1 << ( (coord + 1) % m_interlacingFactor);
                realProjection.insert( coord / m_interlacingFactor );
            }
            return m_baseWeights->getWeight(realProjection) / correctionFactor;
        }

    protected:
        /**
         * Identifies the type of weights, formats them and outputs them on \c os.
         *
         * \remark Deriving classes should identify themselves in the output.
         */
        virtual void format(std::ostream& os) const override
        {
            os << "InterlacedWeightsB(" << m_interlacingFactor << ", " << *m_baseWeights << ")";
        }

    private:
        std::unique_ptr<Weights> m_baseWeights;
        unsigned int m_interlacingFactor;
};

}}

#endif