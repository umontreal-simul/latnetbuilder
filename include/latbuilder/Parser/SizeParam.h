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

#ifndef LATBUILDER__PARSER__SIZE_PARAM_H
#define LATBUILDER__PARSER__SIZE_PARAM_H

#include "latbuilder/Types.h"
#include "latbuilder/SizeParam.h"

namespace LatBuilder { namespace Parser {

/**
 * Parser for size parameters.
 */
struct SizeParam {
   template <LatBuilder::LatType LAT>
   static LatBuilder::SizeParam<LAT> parse(const std::string& str);
};

template <>
LatBuilder::SizeParam<LatBuilder::LatType::ORDINARY> SizeParam::parse(const std::string&);
template <>
LatBuilder::SizeParam<LatBuilder::LatType::EMBEDDED> SizeParam::parse(const std::string&);

extern template LatBuilder::SizeParam<LatBuilder::LatType::ORDINARY> SizeParam::parse<LatBuilder::LatType::ORDINARY>(const std::string&);
extern template LatBuilder::SizeParam<LatBuilder::LatType::EMBEDDED> SizeParam::parse<LatBuilder::LatType::EMBEDDED>(const std::string&);

}}

#endif
