// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#include "latbuilder/Types.h"
#include "latbuilder/Util.h"

namespace LatBuilder {

std::ostream& operator<<(std::ostream& os, EmbeddingType latType)
{ return os << (latType == EmbeddingType::MULTILEVEL ? "embedded" : "ordinary"); }
//============================================================================================================================================

uInteger LatticeTraits<LatticeType::ORDINARY>::ToIndex(const LatticeTraits<LatticeType::ORDINARY>::GenValue& value) {return value;}

LatticeTraits<LatticeType::ORDINARY>::GenValue LatticeTraits<LatticeType::ORDINARY>::ToGenValue(const uInteger& index) {return index;}

uInteger LatticeTraits<LatticeType::ORDINARY>::NumPoints(const LatticeTraits<LatticeType::ORDINARY>::Modulus& modulus){return modulus ;}

uInteger LatticeTraits<LatticeType::ORDINARY>::ToKernelIndex(const size_t& index, const LatticeTraits<LatticeType::ORDINARY>::Modulus& modulus){return index ;}

//=================================================================================================================================================

const LatticeTraits<LatticeType::POLYNOMIAL>::Modulus LatticeTraits<LatticeType::POLYNOMIAL>::TrivialModulus = Polynomial(NTL::INIT_MONO,1);

uInteger LatticeTraits<LatticeType::POLYNOMIAL>::ToIndex(const LatticeTraits<LatticeType::POLYNOMIAL>::GenValue& value) {return IndexOfPolynomial(value);}

LatticeTraits<LatticeType::POLYNOMIAL>::GenValue LatticeTraits<LatticeType::POLYNOMIAL>::ToGenValue(const uInteger& index) {return PolynomialFromInt(index);}

uInteger LatticeTraits<LatticeType::POLYNOMIAL>::NumPoints(const LatticeTraits<LatticeType::POLYNOMIAL>::Modulus& modulus){return intPow(2,deg(modulus));}

uInteger LatticeTraits<LatticeType::POLYNOMIAL>::ToKernelIndex(const size_t& index, const LatticeTraits<LatticeType::POLYNOMIAL>::Modulus& modulus)
	{return Vm(PolynomialFromInt(index),modulus) ;}

} // namespace
