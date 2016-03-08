// Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
// 
// This file is part of Lattice Builder.
// 
// Lattice Builder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Lattice Builder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

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
template <LatType>
struct MeritFilterTraits;

/**
 * Abstract base class for filters.
 */
template <LatType LAT, LatType OUT = LAT>
struct BasicMeritFilter;

/**
 * Exception raised by filters upon rejection of a candidate lattice rule.
 */
struct LatticeRejectedException : std::exception {};


//========================================================================


template <LatType LAT, LatType OUT>
struct BasicMeritFilter {
   typedef typename MeritFilterTraits<LAT>::MeritValue InputMeritValue;
   typedef typename MeritFilterTraits<OUT>::MeritValue OutputMeritValue;
   typedef LatBuilder::LatDef<LAT> LatDef;
   virtual ~BasicMeritFilter() {}
   virtual OutputMeritValue operator() (const InputMeritValue&, const LatDef&) const = 0;
   virtual std::string name() const = 0;
};


//========================================================================


template <> struct MeritFilterTraits<LatType::ORDINARY> {
   typedef Real MeritValue;
};

template <> struct MeritFilterTraits<LatType::EMBEDDED> {
   typedef RealVector MeritValue;
};



}

#endif
