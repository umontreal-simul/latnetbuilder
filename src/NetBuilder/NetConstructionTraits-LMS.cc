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

#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/Helpers/JoeKuo.h"

#include <sstream>
#include <boost/algorithm/string/erase.hpp>


namespace NetBuilder {

    const std::string NetConstructionTraits<NetConstruction::LMS>::name = "LMS";
    
    typedef typename NetConstructionTraits<NetConstruction::LMS>::GenValue GenValue;

    typedef std::vector<uInteger> DirectionNumber;
    typedef std::pair<unsigned int, unsigned int> PrimitivePolynomial; 
    typedef std::vector<std::vector<bool>> BinaryMatrix;

    typedef NetConstructionTraits<NetConstruction::LMS>::SizeParameter SizeParameter;

    bool NetConstructionTraits<NetConstruction::LMS>::checkGenValue(const GenValue& genValue, const SizeParameter& sizeParameter)
    {
        return genValue.nRows() == nRows(sizeParameter) && genValue.nCols() == nCols(sizeParameter);
    }

    unsigned int NetConstructionTraits<NetConstruction::LMS>::nRows(const SizeParameter& sizeParameter) {return (unsigned int) sizeParameter.first.first; }

    unsigned int NetConstructionTraits<NetConstruction::LMS>::nCols(const SizeParameter& sizeParameter) {return (unsigned int) sizeParameter.first.second; }

    GeneratingMatrix*  NetConstructionTraits<NetConstruction::LMS>::createGeneratingMatrix(const GenValue& genValue, const SizeParameter& sizeParameter, const Dimension& dimension_j)
    {
        return new GeneratingMatrix(genValue * *sizeParameter.second[dimension_j]);
    }

    std::vector<GenValue> NetConstructionTraits<NetConstruction::LMS>::genValueSpaceCoord(Dimension coord, const SizeParameter& sizeParameter)
    {
        throw std::logic_error("The space of all matrices is far too big to be exhautively explored.");
        return std::vector<GenValue>{};
    }

    std::vector<std::vector<GenValue>> NetConstructionTraits<NetConstruction::LMS>::genValueSpace(Dimension dimension, const SizeParameter& sizeParameter)
    {
        throw std::logic_error("The space of all matrices is far too big to be exhautively explored.");
        return std::vector<std::vector<GenValue>>{};
    }

    std::string NetConstructionTraits<NetConstruction::LMS>::format(const std::vector<std::shared_ptr<GeneratingMatrix>>& genMatrices, const std::vector<std::shared_ptr<GenValue>>& genVals, const SizeParameter& sizeParameter, OutputStyle outputStyle, unsigned int interlacingFactor)
    {
        return "";
    }  
}
