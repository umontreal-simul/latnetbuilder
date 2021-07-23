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

#include "latticetester/ntlwrap.h"

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
    std::vector<int>*  NetConstructionTraits<NetConstruction::POLYNOMIAL>::createGeneratingVector(const GenValue& genValue, const SizeParameter& sizeParameter)
    {
        // TO DO to return genVector

        unsigned int m = (unsigned int) (deg(sizeParameter));
        std::vector<int>* cj = new std::vector<int>;
       
        for(unsigned int c =0; c<m; c++ )
        {
            cj->push_back(0); 
        }
 
        std::vector<unsigned int> expansion(31 + m);
        expandSeries(genValue, sizeParameter, expansion, 31 + m);
        for(unsigned int c =0; c<m; c++ )
        {
            for(unsigned int row =0; row <31; row++ )
            {
                (*cj)[c] += expansion[c + row]*pow(2, 31-1-row);
            }
        }
        return cj;
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

    std::string NetConstructionTraits<NetConstruction::POLYNOMIAL>::format(const std::vector<std::shared_ptr<GenValue>>& genVals, const SizeParameter& sizeParameter, OutputFormat outputFormat,OutputStyle outputStyle, unsigned int interlacingFactor)
    {
        std::string res;
        std::ostringstream stream;
        //unsigned int m = (unsigned int) (deg(sizeParameter));

        if (outputFormat == OutputFormat::HUMAN){
            stream << "Polynomial Digital Net - Modulus = " << sizeParameter << " - GeneratingVector =" << std::endl;
            for (unsigned int coord = 0; coord < genVals.size(); coord++){
                if (interlacingFactor > 1 && coord % interlacingFactor == 0){
                    stream << "Coordinate " << (coord / interlacingFactor) + 1  << ":" << std::endl;
                }
                stream << "  " << *(genVals[coord]) << std::endl;
            }
        }
        else if (outputFormat == OutputFormat::MACHINE){

            if (outputStyle == OutputStyle::LATTICE){
                stream << "# Parameters for a polynomial lattice rule in base 2, non-embedded" << std::endl;
                /*if (interlacingFactor > 1){
                        stream << ", embedded with interlacing factor " << interlacingFactor << ":" << std::endl;
                }
                else {stream << ", non - embedded" << std::endl;}*/
                stream << genVals.size()/interlacingFactor << "       # "<< genVals.size()/interlacingFactor <<" dimensions" << std::endl;
                stream << (int) deg(sizeParameter) << "      # k = "<<(int) deg(sizeParameter)<< ", n = 2^";
                stream <<  (int) deg(sizeParameter) << " = " << (int)pow(2,deg(sizeParameter) ) << " points"<< std::endl;
                
                NTL::ZZ modulus;
                clear(modulus);
                long degree = deg(sizeParameter);
                for(long i = 0; i <= degree; ++i){
                    modulus += NTL::conv<NTL::ZZ>(sizeParameter[i]) * NTL::power2_ZZ(degree-i);
                }
                stream << modulus << "   # polynomial modulus" << std::endl;
                //stream << sizeParameter << "   # polynomial modulus" << std::endl;

                const GenValue& genValue =*(genVals[0]);
                NTL::ZZ generating_vector;
                clear(generating_vector);
                degree = deg(genValue);
                for(long i = 0; i <= degree; ++i){
                    generating_vector += NTL::conv<NTL::ZZ>(genValue[i]) * NTL::power2_ZZ(degree-i);
                }
                stream << generating_vector << "   # coordinates of generating vector, starting at j=1" <<std::endl;
                //stream << *(genVals[0]) << "   # coordinates of generating vector, starting at j=1" <<std::endl;

                for (unsigned int coord = 1; coord < genVals.size(); coord++){
                    const GenValue& genValue =*(genVals[coord]);
                    NTL::ZZ generating_vector;
                    clear(generating_vector);
                    degree = deg(genValue);
                    for(long i = 0; i <= degree; ++i){
                        generating_vector += NTL::conv<NTL::ZZ>(genValue[i]) * NTL::power2_ZZ(degree-i);
                    }
                    stream << generating_vector << std::endl;
                    //stream << *(genVals[coord]) << std::endl;
                }
                std::string foo(stream.str());
                foo.pop_back();
                stream.str(foo);
                stream.seekp (0, stream.end);
            }
            else if (outputStyle == OutputStyle::NET) {
                stream << "# Parameters for a digital net in base 2 " << std::endl;
                stream<< genVals.size()/interlacingFactor << "    # "<< genVals.size()/interlacingFactor <<" dimensions" << std::endl;
                stream << (int) deg(sizeParameter) << "   # k = "<<(int) deg(sizeParameter) << ", n = 2^"<<  (int) deg(sizeParameter) << " = " << (int)pow(2,deg(sizeParameter) ) << " points"<< std::endl;
                stream << "31   # r = 31 digits" << std::endl;
                stream << "# The next row gives the columns of C_1, the first gen. matrix" << std::endl;
                for(unsigned int coord = 0; coord < genVals.size(); coord++)
                {
                    const std::vector<int>* generatingVector = createGeneratingVector(*(genVals[coord]), sizeParameter);
                    //stream << coord +1 << " ";
                    for(unsigned int i = 0; i < (unsigned int) deg(sizeParameter); ++i)
                    {
                        stream << (*generatingVector)[i] << " ";
                    }
                    stream << std::endl;
                }
                //stream << *last;
                std::string foo(stream.str());
                foo.pop_back();
                stream.str(foo);
                stream.seekp (0, stream.end);
            }
            else{ stream << std::endl;}
        }
        else{ stream << std::endl;}
        res += stream.str();
        boost::algorithm::erase_all(res, "[");
        boost::algorithm::erase_all(res, "]");
        return res;
    }  
}

