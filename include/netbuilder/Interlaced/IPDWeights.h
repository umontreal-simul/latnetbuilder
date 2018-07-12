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
#include "latticetester/ProjectionDependentWeights.h"

#include <memory>
#include <cmath>
#include <iostream>

namespace NetBuilder { namespace Interlaced
{

template <class KERNEL>
class IPDWeights: public LatticeTester::ProjectionDependentWeights
 {
    public:

        IPDWeights(std::unique_ptr<LatticeTester::ProjectionDependentWeights> weights, const KERNEL& kernel);

        virtual void setWeight (const LatticeTester::Coordinates & projection, double weight);

    private:
        std::string m_KernelName;

    protected:
        /**
         * Identifies the type of weights, formats them and outputs them on \c os.
         *
         * \remark Deriving classes should identify themselves in the output.
         */
        virtual void format(std::ostream& os) const override;

};

extern template class IPDWeights<LatBuilder::Kernel::AIDNAlpha>;
extern template class IPDWeights<LatBuilder::Kernel::BIDN>;

}}

#endif