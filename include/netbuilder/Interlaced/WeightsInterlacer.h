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

#ifndef NETBUILDER__INTERLACED__WEIGHTS_INTERLACER_H
#define NETBUILDER__INTERLACED__WEIGHTS_INTERLACER_H

#include "latbuilder/CombinedWeights.h"
#include "latbuilder/WeightsDispatcher.h"

#include "latticetester/Weights.h"
#include "latticetester/OrderDependentWeights.h"
#include "latticetester/ProductWeights.h"
#include "latticetester/PODWeights.h"
#include "latticetester/ProjectionDependentWeights.h"
#include "netbuilder/Interlaced/InterlacedWeights.h"
#include "netbuilder/Interlaced/IPODWeights.h"

#include <memory>
#include <sstream>
#include <string>

namespace NetBuilder { namespace Interlaced {

template<typename KERNEL, typename WEIGHTS>
struct WeightsInterlacerHelper {
    std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<WEIGHTS> w, const KERNEL& kernel) const
    { 
        return std::make_unique<InterlacedWeights<KERNEL>>(std::move(w), kernel);
    }
};

template <typename KERNEL>
struct WeightsInterlacerHelper<KERNEL, LatticeTester::ProductWeights> {
    std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<LatticeTester::ProductWeights> w, const KERNEL& kernel) const
    { 
        return std::make_unique<IPODWeights<KERNEL>>(std::move(w), kernel);
    }
};

template <typename KERNEL>
struct WeightsInterlacerHelper<KERNEL, LatticeTester::OrderDependentWeights> {
    std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<LatticeTester::OrderDependentWeights> w, const KERNEL& kernel) const
    { 
        return std::make_unique<IPODWeights<KERNEL>>(std::move(w), kernel);
    }
};

template <typename KERNEL>
struct WeightsInterlacerHelper<KERNEL, LatticeTester::PODWeights> {
    std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<LatticeTester::PODWeights> w, const KERNEL& kernel) const
    { 
        return std::make_unique<IPODWeights<KERNEL>>(std::move(w), kernel);
    }
};

template<typename KERNEL>
struct WeightsInterlacerContainer{
    template <typename WEIGHTS>
    struct WeightsInterlacer {

        std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<WEIGHTS> w, const KERNEL& kernel) const
        { 
            return WeightsInterlacerHelper<KERNEL, WEIGHTS>()(std::move(w), kernel);
        }
    };
};

// template <typename KERNEL>
// std::unique_ptr<LatticeTester::Weights> WeightsInterlacerContainer<KERNEL>:: template<> WeightsInterlacer<LatticeTester::ProductWeights>::operator()(std::unique_ptr<LatticeTester::ProductWeights> w, const KERNEL& kernel) const
//     { 
//         return std::make_unique<IPODWeights<KERNEL>>(std::move(w), kernel);
//     }

// template <typename KERNEL>
// std::unique_ptr<LatticeTester::Weights> WeightsInterlacerContainer<KERNEL>:: template<> WeightsInterlacer<LatticeTester::OrderDependentWeights>:: operator()(std::unique_ptr<LatticeTester::OrderDependentWeights> w, const KERNEL& kernel) const
//     { 
//         return std::make_unique<IPODWeights<KERNEL>>(std::move(w), kernel);
//     }

// template <typename KERNEL>
// std::unique_ptr<LatticeTester::Weights> WeightsInterlacerContainer<KERNEL>:: template<> WeightsInterlacer<LatticeTester::PODWeights>::operator()(std::unique_ptr<LatticeTester::PODWeights> w, const KERNEL& kernel) const
//     { 
//         return std::make_unique<IPODWeights<KERNEL>>(std::move(w), kernel);
//     }





// #define WEIGHTS_INTERLACER(KERNEL)\
// template <typename WEIGHTS>\
// struct WeightsInterlacer {\
//     std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<WEIGHTS> w, const KERNEL& kernel) const\
//     { \
//         return std::make_unique<InterlacedWeights<KERNEL>>(std::move(w), kernel);\
//     }\
// };\
// \
// template <>\
// struct WeightsInterlacer<LatticeTester::ProductWeights, KERNEL> {\
//     std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<LatticeTester::ProductWeights> w, const KERNEL& kernel) const\
//     { \
//         return std::make_unique<IPODWeights<KERNEL>>(std::move(w), kernel);\
//     }\
// };\
// \
// template <>\
// struct WeightsInterlacer<LatticeTester::OrderDependentWeights, KERNEL> {\
//     std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<LatticeTester::OrderDependentWeights> w, const KERNEL& kernel) const\
//     { \
//         return std::make_unique<IPODWeights<KERNEL>>(std::move(w), kernel);\
//     }\
// };\
// \
// template <>\
// struct WeightsInterlacer<LatticeTester::PODWeights, KERNEL> {\
//     std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<LatticeTester::PODWeights> w, const KERNEL& kernel) const\
//     { \
//         return std::make_unique<IPODWeights<KERNEL>>(std::move(w), kernel);\
//     }\
// };\
// \
// template <>\
// struct WeightsInterlacer<LatBuilder::CombinedWeights> {\
//     std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<LatBuilder::CombinedWeights> cw, const KERNEL& kernel) const\
//     { \
//         auto res = std::make_unique<LatBuilder::CombinedWeights>();\
//         for (auto& w: cw->list()){\
//             auto foo = LatBuilder::WeightsDispatcher::dispatchPtr<WeightsInterlacer>(std::move(w), kernel);\
//             res->add(std::move(foo));\
//         }\
//         return std::move(res);\
//     }\
// };\

// WEIGHTS_INTERLACER(LatBuilder::Kernel::AIDNAlpha);
// WEIGHTS_INTERLACER(LatBuilder::Kernel::BIDN);

// #undef WEIGHTS_INTERLACER

}}

#endif