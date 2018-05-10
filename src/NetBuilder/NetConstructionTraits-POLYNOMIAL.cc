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

#if 0

#include "netbuilder/Types.h"
#include "netbuilder/GeneratingMatrix.h"
#include "netbuilder/NetConstructionTraits.h"

#include "latbuilder/GeneratingValues-PLR.h"
#include "latbuilder/SeqCombiner.h"

#include <NTL/GF2X.h>

namespace NetBuilder {

    
    typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::GenValue GenValue;

    bool NetConstructionTraits<NetConstruction::POLYNOMIAL>::checkGenValue(const GenValue& genValue)
    {
        return isOne(GCD(genValue.first,genValue.second));
    }

    GeneratingMatrix*  NetConstructionTraits<NetConstruction::POLYNOMIAL>::createGeneratingMatrix(const GenValue& genValue, unsigned int nRows, unsigned int nCols) 
    {
    }

    std::vector<GenValue> NetConstructionTraits<NetConstruction::POLYNOMIAL>::defaultGenValues(unsigned int dimension){
        std::vector<std::vector<uInteger>> tmp = readJoeKuoDirectionNumbers(dimension);
        std::vector<GenValue> res(dimension);
        for(unsigned int j = 0; j < dimension; ++j)
        {
            res[j] = GenValue(j+1,tmp[j]);
        }
        return res;
    }

    std::vector<GenValue> NetConstructionTraits<NetConstruction::POLYNOMIAL>::genValueSpaceDim(unsigned int dimension)
    {
        LatBuilder::GenSeq::GeneratingValues<LatBuilder::LatticeType::POLYNOMIAL,LatBuilder::Compress:NONE,LatBuilder::Traversal::Forward>
        unsigned int size;
        if (dimension==1)
        {
            size = 1;
        }
        else
        {
            size = nthPrimitivePolynomialDegree(dimension-1);
        }
        std::vector<SobolDirectionNumbers<>> seqs;
        seqs.reserve(size);
        for(unsigned int i = 0; i < size; ++i)
        {
            seqs.push_back(SobolDirectionNumbers<>(i+1));
        }
        LatBuilder::SeqCombiner<SobolDirectionNumbers<>,LatBuilder::CartesianProduct> tmp(seqs);

        std::vector<GenValue> res;
        for(const auto& x : tmp)
        {
            res.push_back(GenValue(dimension,x));
        }
        return res;
    }

    std::vector<std::vector<GenValue>> NetConstructionTraits<NetConstruction::POLYNOMIAL>::genValueSpace(unsigned int maxDimension)
    {
        std::vector<std::vector<GenValue>> seqs;
        seqs.reserve(maxDimension);
        for(unsigned int i = 0; i < maxDimension; ++i)
        {
            seqs.push_back(genValueSpaceDim(i+1));
        }
        LatBuilder::SeqCombiner<std::vector<GenValue>, LatBuilder::CartesianProduct> tmp(seqs);
        std::vector<std::vector<GenValue>> res;
        for(const auto& foo : tmp)
        {
            res.push_back(foo);
        }
        return res;
    }

    void NetConstructionTraits<NetConstruction::POLYNOMIAL>::extendGeneratingMatrices( 
        unsigned int inc,
        const std::vector<std::shared_ptr<GeneratingMatrix>>& genMats, 
        const std::vector<std::shared_ptr<GenValue>>& genValues)
    {
        throw std::logic_error("Polynomial lattices cannot be viewed as digital sequences")
    }
}

#endif