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

#include "latbuilder/WeightsDispatcher.h"
#include "netbuilder/Interlaced/IPDWeights.h"
#include <iostream>

namespace NetBuilder { namespace Interlaced {

/**
 * Class to interlace weights in dimension \f$s\f$ into weights in dimension \f$d s\f$ using the \f$w\f$ operator defined in
 * Corollary 1. in \cite rGOD13a. This operation depends on the kernel.
 * @tparam WEIGHTS Type of weights to interlace.
 * This class is designed to be used with LatBuider::WeightsDispatcher::dispatchPtr.
 */ 
template<typename WEIGHTS> struct WeightsInterlacer{
    /**
     * Interlaces the weights \c w using the kernel \c kernel.
     * @tparam KERNEL Type of the kernel.
     * @param w Weights.
     * @param kernel Kernel.
     */ 
    template <typename KERNEL>
    std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<WEIGHTS> w, const KERNEL& kernel) const;
};

namespace {
    template<typename KERNEL, typename WEIGHTS>
    struct WeightsInterlacerHelper {
        std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<WEIGHTS> w, const KERNEL& kernel) const
        { 
            throw runtime_error("General weights cannot be interlaced.");
            std::unique_ptr<LatticeTester::Weights> res;
            return res;
        }
    };

    template<typename KERNEL>
    struct WeightsInterlacerHelper<KERNEL, LatticeTester::ProjectionDependentWeights> {
        std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<LatticeTester::ProjectionDependentWeights> w, const KERNEL& kernel) const
        { 
            return std::make_unique<IPDWeights<KERNEL>>(std::move(w), kernel);
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

    template <typename KERNEL>
    struct WeightsInterlacerHelper<KERNEL, LatBuilder::CombinedWeights> {
        std::unique_ptr<LatticeTester::Weights> operator()(std::unique_ptr<LatBuilder::CombinedWeights> cw, const KERNEL& kernel) const
        { 
            auto res = std::make_unique<LatBuilder::CombinedWeights>();
            for (auto& w: cw->giveWeights()){
                auto foo = LatBuilder::WeightsDispatcher::dispatchPtr<WeightsInterlacer>(std::move(w), kernel);
                res->add(std::move(foo));
            }
            return res;
        }
    };
}

template <typename WEIGHTS>
template <typename KERNEL>
std::unique_ptr<LatticeTester::Weights> WeightsInterlacer<WEIGHTS>:: operator()(std::unique_ptr<WEIGHTS> w, const KERNEL& kernel) const
{ 
    return WeightsInterlacerHelper<KERNEL, WEIGHTS>()(std::move(w), kernel);
}


}}

#endif