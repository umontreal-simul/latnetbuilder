// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
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

#ifndef DIGITAL_NET_GAUSS_METHOD_H
#define DIGITAL_NET_GAUSS_METHOD_H

#include "latbuilder/DigitalNet/DigitalNet.h"

namespace LatBuilder { namespace DigitalNet {

    struct GaussMethod{
    // compute the t-value of the set of matrices Origin_Mats with `maxSubProj' the maximum of
    // the t-value of all strict subprojections
    static int computeTValue(std::vector<Matrix<2>> Origin_Mats, int maxSubProj, bool verbose);
};
}}

#endif


