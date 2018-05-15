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

#ifndef NETBUILDER__COMPUTE_MAX_CARD_FROM_WEIGHTS_H
#define NETBUILDER__COMPUTE_MAX_CARD_FROM_WEIGHTS_H

#include "latcommon/OrderDependentWeights.h"
#include "latcommon/ProductWeights.h"
#include "latcommon/PODWeights.h"
#include "latcommon/ProjectionDependentWeights.h"

/**
 * Helper structure template to compute the maximal order of subprojections
 * with non zero weights when possible.
 */ 
template <typename WEIGHTS>
struct ComputeMaxCardFromWeights {
    unsigned int operator()(const WEIGHTS& w) const
    { 
        throw std::invalid_argument("weights cannot be of type " + w.name() );
        return 0;
    }
};

#define DECLARE_COMPUTE_MAX_CARD(weight_type) \
    template <>\
    struct ComputeMaxCardFromWeights<weight_type> { \
    unsigned int operator()(const weight_type& w) const;\
    }

   DECLARE_COMPUTE_MAX_CARD(LatCommon::OrderDependentWeights);
   DECLARE_COMPUTE_MAX_CARD(LatCommon::ProductWeights);
   DECLARE_COMPUTE_MAX_CARD(LatCommon::PODWeights);
   DECLARE_COMPUTE_MAX_CARD(LatCommon::ProjectionDependentWeights);

#undef DECLARE_COMPUTE_MAX_CARD

/**
 * Template specialization in the case of OrderDependentWeights
 */ 
unsigned int ComputeMaxCardFromWeights<LatCommon::OrderDependentWeights>::operator()(const LatCommon::OrderDependentWeights& w) const{
    if (w.getDefaultWeight() > 0){
        throw std::invalid_argument("default weight must be zero to use this figure." );
    }
    unsigned int maxCard = 0;
    for (unsigned int i=1; i<w.getSize(); i++){
        if (w.getWeightForOrder(i) != 0){
            maxCard = i;
        }
    }
    return maxCard;
}

/**
 * Template specialization in the case of OrderDependentWeights
 */ 
unsigned int ComputeMaxCardFromWeights<LatCommon::ProductWeights>::operator()(const LatCommon::ProductWeights& w) const{
    if (w.getDefaultWeight() > 0){
        throw std::invalid_argument("default weight must be zero to use this figure." );
    }
    return (unsigned int) w.getWeights().size();
}

/**
 * Template specialization in the case of PODWeights.
 */ 
unsigned int ComputeMaxCardFromWeights<LatCommon::PODWeights>::operator()(const LatCommon::PODWeights& w) const{
    const LatCommon::OrderDependentWeights& ordDepWeights = w.getOrderDependentWeights();
    const LatCommon::ProductWeights& prodWeights = w .getProductWeights();
    auto computer1 = ComputeMaxCardFromWeights<LatCommon::OrderDependentWeights>();
    auto computer2 = ComputeMaxCardFromWeights<LatCommon::ProductWeights>();
    return std::max(computer1(ordDepWeights),computer2(prodWeights));
}

/**
 * Template specialization in the case of ProjectionDependentWeights.
 */ 
unsigned int ComputeMaxCardFromWeights<LatCommon::ProjectionDependentWeights>::operator()(const LatCommon::ProjectionDependentWeights& w) const{
    unsigned int maxCard = 0;
    for (unsigned int i=0; i<w.getSize(); i++){
        for (const auto &kv : w.getWeightsForLargestIndex(i)){
            maxCard = std::max(maxCard, (unsigned int) kv.first.size());
        }
    }
    return maxCard;
}

#endif
