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

#include "latbuilder/Types.h"
#include "latbuilder/Util.h"

namespace LatBuilder {

std::ostream& operator<<(std::ostream& os, LatType latType)
{ return os << (latType == LatType::EMBEDDED ? "embedded" : "ordinary"); }
//============================================================================================================================================

uInteger LatticeTraits<Lattice::INTEGRATION>::ToIndex(const LatticeTraits<Lattice::INTEGRATION>::GenValue& value) {return value;}

LatticeTraits<Lattice::INTEGRATION>::GenValue LatticeTraits<Lattice::INTEGRATION>::ToGenValue(const uInteger& index) {return index;}

uInteger LatticeTraits<Lattice::INTEGRATION>::NumPoints(const LatticeTraits<Lattice::INTEGRATION>::Modulus& modulus){return modulus ;}

uInteger LatticeTraits<Lattice::INTEGRATION>::ToKernelIndex(const size_t& index, const LatticeTraits<Lattice::INTEGRATION>::Modulus& modulus){return index ;}

//=================================================================================================================================================

const LatticeTraits<Lattice::POLYNOMIAL>::Modulus LatticeTraits<Lattice::POLYNOMIAL>::TrivialModulus = Polynomial(INIT_MONO,1);

uInteger LatticeTraits<Lattice::POLYNOMIAL>::ToIndex(const LatticeTraits<Lattice::POLYNOMIAL>::GenValue& value) {return IndexOfPolynomial(value);}

LatticeTraits<Lattice::POLYNOMIAL>::GenValue LatticeTraits<Lattice::POLYNOMIAL>::ToGenValue(const uInteger& index) {return PolynomialFromInt(index);}

uInteger LatticeTraits<Lattice::POLYNOMIAL>::NumPoints(const LatticeTraits<Lattice::POLYNOMIAL>::Modulus& modulus){return intPow(2,deg(modulus));}

uInteger LatticeTraits<Lattice::POLYNOMIAL>::ToKernelIndex(const size_t& index, const LatticeTraits<Lattice::POLYNOMIAL>::Modulus& modulus)
	{return Vm(PolynomialFromInt(index),modulus) ;}

} // namespace
