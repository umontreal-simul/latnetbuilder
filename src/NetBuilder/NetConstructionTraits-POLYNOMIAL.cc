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

    
    typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::GenValue GenValue;

    typedef typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::DesignParameter DesignParameter;

    DesignParameter NetConstructionTraits<NetConstruction::POLYNOMIAL>::defaultDesignParameter(1);

    bool NetConstructionTraits<NetConstruction::POLYNOMIAL>::checkGenValue(const GenValue& genValue, const DesignParameter& designParameter)
    {
        return IsOne(GCD(genValue,designParameter));
    }

    unsigned int NetConstructionTraits<NetConstruction::POLYNOMIAL>::nRows(const DesignParameter& designParameter) {return (unsigned int) deg(designParameter); }

    unsigned int NetConstructionTraits<NetConstruction::POLYNOMIAL>::nCols(const DesignParameter& designParameter) {return (unsigned int) deg(designParameter); }


    void expandSeries(const GenValue& genValue, const DesignParameter& designParameter, std::vector<unsigned int>& expansion, unsigned int expansion_limit){
        int m = (int) deg(designParameter); 
        for(int l = 1; l<= expansion_limit ; l++){
            int res =  (m-l >=0 && IsOne(coeff(genValue, m-l)))? 1 : 0;
            int start = (l-m > 1) ? (l-m) : 1;
            for( int p = start; p < l; p++){
                res = ( res + expansion[p-1] * conv<int>(coeff(designParameter, m-(l-p)))) %2;        
            }
            expansion[l-1] = res;
        }
    }

    GeneratingMatrix*  NetConstructionTraits<NetConstruction::POLYNOMIAL>::createGeneratingMatrix(const GenValue& genValue, const DesignParameter& designParameter)
    {
        unsigned int m = (unsigned int) (deg(designParameter));
        GeneratingMatrix* genMat = new GeneratingMatrix(m,m);
        std::vector<unsigned int> expansion(2 * m);
        expandSeries(genValue, designParameter, expansion, 2 * m);
        for(unsigned int c =0; c<m; c++ )
        {
            for(unsigned int row =0; row <m; row++ )
            {
                (*genMat)(row,c) = expansion[c + row];
            }
        }
        return genMat;
    }

    std::vector<GenValue> NetConstructionTraits<NetConstruction::POLYNOMIAL>::defaultGenValues(Dimension dimension, const DesignParameter& designParameter)
    {
        std::vector<GenValue> res(dimension);
        for(unsigned int j = 0; j < dimension; ++j)
        {
            res[j] = GenValue(1);
        }
        return res;
    }

    typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::GenValueSpaceCoordSeq NetConstructionTraits<NetConstruction::POLYNOMIAL>::genValueSpaceCoord(Dimension coord, const DesignParameter& designParameter)
    {
        if (coord==0)
        {
            Polynomial fakeModulus;
            SetX(fakeModulus);
            return GenValueSpaceCoordSeq(fakeModulus);
        }
        else
        {
           return GenValueSpaceCoordSeq(designParameter);
        }
    }

    typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::GenValueSpaceSeq NetConstructionTraits<NetConstruction::POLYNOMIAL>::genValueSpace(Dimension dimension, const DesignParameter& designParameter)
    {
        std::vector<GenValueSpaceCoordSeq> seqs;
        seqs.reserve(dimension);
        for(Dimension coord = 0; coord < dimension; ++coord)
        {
            seqs.push_back(genValueSpaceCoord(coord, designParameter));
        }
        return GenValueSpaceSeq(seqs);
    }

    std::string NetConstructionTraits<NetConstruction::POLYNOMIAL>::format(const std::vector<std::shared_ptr<GenValue>>& genVals, const DesignParameter& designParameter, OutputFormat outputFormat)
    {
        std::string res;
        
        std::ostringstream stream;
        res += "PolynomialDigitalNet(\n  Modulus = \n";
        stream << designParameter;
        res+="  ";
        res += stream.str();
        stream.str(std::string());
        res += "\n  GeneratingVector = \n";
        for(const auto& genVal : genVals)
        {
            res+="  ";
            stream << *genVal;
            res+= stream.str();
            stream.str(std::string());
            res+= "\n";
        }
        res+=")";
        boost::algorithm::erase_all(res, "[");
        boost::algorithm::erase_all(res, "]");
        return res;
    }  
}

