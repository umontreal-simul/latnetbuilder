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

#include "netbuilder/Accumulator.h"

#include <limits>

namespace NetBuilder{

Real AddBinaryOperator::operator()(Real x, Real y){return x+y;}

Real MaxBinaryOperator::operator()(Real x, Real y){ return (x < y) ? y : x;}

Accumulator::Accumulator(Real initialValue, Real normType):
            m_data(initialValue),
            m_op(realToBinOp(normType))
{};

void Accumulator::accumulate(Real weight, Real value, Real power){
    m_data = m_op(weight*std::pow(value,power), m_data);
}

Real Accumulator::tryAccumulate(Real weight, Real value, Real power) const {
    return m_op(weight*std::pow(value,power), m_data);
}

void Accumulator::set(Real value) { m_data = value; }

Real Accumulator::value() const
{
    return m_data;
}

std::function<Real (Real, Real)>  Accumulator::realToBinOp(Real normType){
    if (normType < std::numeric_limits<Real>::infinity())
    {
        return std::function<Real (Real, Real)> (AddBinaryOperator());
    }
    return std::function<Real (Real, Real)> (MaxBinaryOperator());
}

}