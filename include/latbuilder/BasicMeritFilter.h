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

#ifndef LATBUILDER__BASIC_MERIT_FILTER_H
#define LATBUILDER__BASIC_MERIT_FILTER_H

#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"

namespace LatBuilder {

/**
 * Traits class template for filters of merit values.
 *
 * It defines MeritValue as the type of merit value associated to a given type
 * of lattice.
 */
template <PointSetType>
struct MeritFilterTraits;

/**
 * Abstract base class for filters.
 */
template <LatticeType LR, PointSetType PST, PointSetType OUT = PST>
struct BasicMeritFilter;

/**
 * Exception raised by filters upon rejection of a candidate lattice rule.
 */
struct LatticeRejectedException : std::exception {};


//========================================================================


template <LatticeType LR, PointSetType PST, PointSetType OUT>
struct BasicMeritFilter {
   typedef typename MeritFilterTraits<PST>::MeritValue InputMeritValue;
   typedef typename MeritFilterTraits<OUT>::MeritValue OutputMeritValue;
   typedef LatBuilder::LatDef<LR, PST> LatDef;
   virtual ~BasicMeritFilter() {}
   virtual OutputMeritValue operator() (const InputMeritValue&, const LatDef&) const = 0;
   virtual std::string name() const = 0;
};


//========================================================================


template <> struct MeritFilterTraits<PointSetType::UNILEVEL> {
   typedef Real MeritValue;
};

template <> struct MeritFilterTraits<PointSetType::MULTILEVEL> {
   typedef RealVector MeritValue;
};



}

#endif
