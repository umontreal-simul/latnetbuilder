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

#include <sstream>
#include <boost/algorithm/string/erase.hpp>

namespace NetBuilder {

    
    typedef typename NetConstructionTraits<NetConstruction::EXPLICIT>::GenValue GenValue;

    typedef NetConstructionTraits<NetConstruction::EXPLICIT>::DesignParameter DesignParameter;

    DesignParameter NetConstructionTraits<NetConstruction::EXPLICIT>::defaultDesignParameter(0,0);

    bool NetConstructionTraits<NetConstruction::EXPLICIT>::checkGenValue(const GenValue& genValue, const DesignParameter& designParameter)
    {
        return genValue.nRows() == nRows(designParameter) && genValue.nCols() == nCols(designParameter);
    }

    unsigned int NetConstructionTraits<NetConstruction::EXPLICIT>::nRows(const DesignParameter& designParameter) {return (unsigned int) designParameter.first; }

    unsigned int NetConstructionTraits<NetConstruction::EXPLICIT>::nCols(const DesignParameter& designParameter) {return (unsigned int) designParameter.second; }

    GeneratingMatrix*  NetConstructionTraits<NetConstruction::EXPLICIT>::createGeneratingMatrix(const GenValue& genValue, const DesignParameter& designParameter, std::shared_ptr<GeneratingMatrixComputationData>& computationData)
    {
        GeneratingMatrix* genMat = new GeneratingMatrix(genValue);
        genMat->resize(nRows(designParameter),nCols(designParameter));
        return genMat;
    }

    std::vector<GenValue> NetConstructionTraits<NetConstruction::EXPLICIT>::defaultGenValues(Dimension dimension, const DesignParameter& designParameter)
    {
        GeneratingMatrix identity(nRows(designParameter), nCols(designParameter));
        for(unsigned int i = 0; i < nRows(designParameter); ++i)
        {
            identity.flip(i,i);
        }
        return std::vector<GeneratingMatrix>(dimension,identity);
    }

    std::vector<GenValue> NetConstructionTraits<NetConstruction::EXPLICIT>::genValueSpaceDim(Dimension dimension, const DesignParameter& designParameter)
    {
        throw std::logic_error("The space of all matrices is far too big to be exhautively explored.");
        return std::vector<GenValue>{};
    }

    std::vector<std::vector<GenValue>> NetConstructionTraits<NetConstruction::EXPLICIT>::genValueSpace(Dimension maxDimension, const DesignParameter& designParameter)
    {
        throw std::logic_error("The space of all matrices is far too big to be exhautively explored.");
        return std::vector<std::vector<GenValue>>{};
    }

    std::string NetConstructionTraits<NetConstruction::EXPLICIT>::format(const std::vector<std::shared_ptr<GenValue>>& genVals, const DesignParameter& designParameter, OutputFormat outputFormat)
    {
        std::string res;
        
        std::ostringstream stream;
        res += "ExplicitDigitalNet(\n  Matrix size = \n";
        stream << designParameter.first << " " << designParameter.second;
        res+="  ";
        res += stream.str();
        stream.str(std::string());
        res+="\n)";
        return res;
    }  
}