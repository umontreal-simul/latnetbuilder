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

#ifndef LATBUILDER__PARSER__SIZE_PARAM_H
#define LATBUILDER__PARSER__SIZE_PARAM_H

#include "latbuilder/Types.h"
#include "latbuilder/SizeParam.h"

namespace LatBuilder { namespace Parser {

/**
 * Parser for size parameters.
 */
	template <Lattice LR, LatBuilder::LatType LAT>
struct SizeParam {
   
   static LatBuilder::SizeParam<LR, LAT> parse(const std::string& str);
};

/*
template <>
LatBuilder::SizeParam<Lattice LatBuilder::LatType::ORDINARY> SizeParam::parse(const std::string&);
template <>
LatBuilder::SizeParam<LatBuilder::LatType::EMBEDDED> SizeParam::parse(const std::string&);
*/
/*
extern template LatBuilder::SizeParam<Lattice::INTEGRATION, LatBuilder::LatType::ORDINARY> 
			LatBuilder::Parser::SizeParam<Lattice::INTEGRATION, LatBuilder::LatType::ORDINARY>::parse(const std::string& str);

extern template LatBuilder::SizeParam<Lattice::INTEGRATION, LatBuilder::LatType::EMBEDDED> 
			LatBuilder::Parser::SizeParam<Lattice::INTEGRATION, LatBuilder::LatType::EMBEDDED>::parse(const std::string& str);

extern template LatBuilder::SizeParam<Lattice::POLYNOMIAL, LatBuilder::LatType::ORDINARY> 
			LatBuilder::Parser::SizeParam<Lattice::POLYNOMIAL, LatBuilder::LatType::ORDINARY>::parse(const std::string& str);

extern template LatBuilder::SizeParam<Lattice::POLYNOMIAL, LatBuilder::LatType::EMBEDDED> 
			LatBuilder::Parser::SizeParam<Lattice::POLYNOMIAL, LatBuilder::LatType::EMBEDDED>::parse(const std::string& str);


extern template struct LatBuilder::Parser::SizeParam<Lattice::INTEGRATION, LatBuilder::LatType::ORDINARY>;
extern template struct LatBuilder::Parser::SizeParam<Lattice::INTEGRATION, LatBuilder::LatType::EMBEDDED>;

extern template struct LatBuilder::Parser::SizeParam<Lattice::POLYNOMIAL, LatBuilder::LatType::ORDINARY>;
extern template struct LatBuilder::Parser::SizeParam<Lattice::POLYNOMIAL, LatBuilder::LatType::EMBEDDED>;
*/
}}

#endif
