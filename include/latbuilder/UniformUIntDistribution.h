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

#ifndef LATBUILDER__UNIFORM_UINT_DISTRIBUTION_H
#define LATBUILDER__UNIFORM_UINT_DISTRIBUTION_H

#include <limits>
#include <array>
#include <stdexcept>
#include <cstdint>

namespace LatBuilder {

template <typename UINTTYPE, typename RAND>
class UniformUIntDistribution
{
    public:
        UniformUIntDistribution(UINTTYPE a = 0, UINTTYPE b = 1):
            m_lower(a),
            m_upper(b),
            m_range((b >= a) ? (b - a + 1) : 0)
        {}

        UINTTYPE operator()(RAND& rand)
        {
            if (m_range == 0)
            {
                return m_upper;
            }
            else
            {
                UINTTYPE reject_limit = rand.max() % m_range;
                UINTTYPE n;
                do
                {
                        n = rand();
                }
                while (n <= reject_limit);
                return (n % m_range) + m_lower;
            }
        }

        void setLowerBound(UINTTYPE a)
        {
            setBounds(a, m_upper);
        }

        void setUpperBound(UINTTYPE b)
        {
            setBounds(m_lower, b);
        }

        void setBounds(UINTTYPE a, UINTTYPE b)
        {
            m_lower = a;
            m_upper = b;
            m_range = m_upper - m_lower + 1;
        }

    private:
        UINTTYPE m_lower;
        UINTTYPE m_upper;
        UINTTYPE m_range;
};

}
#endif
