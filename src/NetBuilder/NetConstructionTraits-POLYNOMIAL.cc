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

#include "latbuilder/GenSeq/GeneratingValues-PLR.h"
#include "latbuilder/SeqCombiner.h"

#include <NTL/GF2X.h>
#include <sstream>
#include <boost/algorithm/string/erase.hpp>

namespace NetBuilder {

    const std::string NetConstructionTraits<NetConstruction::POLYNOMIAL>::name = "Polynomial";
    
    typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::GenValue GenValue;

    typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::SizeParameter SizeParameter;

    bool NetConstructionTraits<NetConstruction::POLYNOMIAL>::checkGenValue(const GenValue& genValue, const SizeParameter& sizeParameter)
    {
        return IsOne(GCD(genValue,sizeParameter));
    }

    unsigned int NetConstructionTraits<NetConstruction::POLYNOMIAL>::nRows(const SizeParameter& sizeParameter) {return (unsigned int) deg(sizeParameter); }

    unsigned int NetConstructionTraits<NetConstruction::POLYNOMIAL>::nCols(const SizeParameter& sizeParameter) {return (unsigned int) deg(sizeParameter); }


    void expandSeries(const GenValue& genValue, const SizeParameter& sizeParameter, std::vector<unsigned int>& expansion, unsigned int expansion_limit){
        int m = (int) deg(sizeParameter); 
        for(int l = 1; l<= (int) expansion_limit ; l++){
            int res =  (m-l >=0 && IsOne(coeff(genValue, m-l)))? 1 : 0;
            int start = (l-m > 1) ? (l-m) : 1;
            for( int p = start; p < l; p++){
                res = ( res + expansion[p-1] * NTL::conv<int>(coeff(sizeParameter, m-(l-p)))) %2;        
            }
            expansion[l-1] = res;
        }
    }

    GeneratingMatrix*  NetConstructionTraits<NetConstruction::POLYNOMIAL>::createGeneratingMatrix(const GenValue& genValue, const SizeParameter& sizeParameter)
    {
        unsigned int m = (unsigned int) (deg(sizeParameter));
        GeneratingMatrix* genMat = new GeneratingMatrix(m,m);
        std::vector<unsigned int> expansion(2 * m);
        expandSeries(genValue, sizeParameter, expansion, 2 * m);
        for(unsigned int c =0; c<m; c++ )
        {
            for(unsigned int row =0; row <m; row++ )
            {
                (*genMat)(row,c) = expansion[c + row];
            }
        }
        return genMat;
    }

    typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::GenValueSpaceCoordSeq NetConstructionTraits<NetConstruction::POLYNOMIAL>::genValueSpaceCoord(Dimension coord, const SizeParameter& sizeParameter)
    {
        if (coord==0)
        {
            Polynomial fakeModulus;
            SetX(fakeModulus);
            return GenValueSpaceCoordSeq(fakeModulus);
        }
        else
        {
           return GenValueSpaceCoordSeq(sizeParameter);
        }
    }

    typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::GenValueSpaceSeq NetConstructionTraits<NetConstruction::POLYNOMIAL>::genValueSpace(Dimension dimension, const SizeParameter& sizeParameter)
    {
        std::vector<GenValueSpaceCoordSeq> seqs;
        seqs.reserve(dimension);
        for(Dimension coord = 0; coord < dimension; ++coord)
        {
            seqs.push_back(genValueSpaceCoord(coord, sizeParameter));
        }
        return GenValueSpaceSeq(seqs);
    }

    std::string NetConstructionTraits<NetConstruction::POLYNOMIAL>::format(const std::vector<std::shared_ptr<GenValue>>& genVals, const SizeParameter& sizeParameter, OutputFormat outputFormat, unsigned int interlacingFactor)
    {
        std::string res;
        std::ostringstream stream;

        if (outputFormat == OutputFormat::HUMAN){
            stream << "Polynomial Digital Net - Modulus = " << sizeParameter << " - GeneratingVector =" << std::endl;
            for (unsigned int coord = 0; coord < genVals.size(); coord++){
                if (interlacingFactor > 1 && coord % interlacingFactor == 0){
                    stream << "Coordinate " << (coord / interlacingFactor) + 1  << ":" << std::endl;
                }
                stream << "  " << *(genVals[coord]) << std::endl;
            }
        }
        else{
            stream << "Polynomial  // Construction method" << std::endl;
            stream << sizeParameter << "  // modulus" << std::endl;
            for (unsigned int coord = 0; coord < genVals.size(); coord++){
                stream << *(genVals[coord]) << std::endl;
            }
        }

        res += stream.str();
        boost::algorithm::erase_all(res, "[");
        boost::algorithm::erase_all(res, "]");
        return res;
    }  
}

