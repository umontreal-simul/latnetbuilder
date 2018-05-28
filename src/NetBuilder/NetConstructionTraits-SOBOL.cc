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

#include "netbuilder/Types.h"
#include "netbuilder/GeneratingMatrix.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/SobolDirectionNumbers.h"

#include "latbuilder/SeqCombiner.h"
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <list>
#include <assert.h>

#include "latbuilder/TextStream.h"

namespace NetBuilder {

    
    typedef typename NetConstructionTraits<NetConstruction::SOBOL>::GenValue GenValue;

    typedef typename NetConstructionTraits<NetConstruction::SOBOL>::DesignParameter DesignParameter;

    DesignParameter NetConstructionTraits<NetConstruction::SOBOL>::defaultDesignParameter = 1;

    bool NetConstructionTraits<NetConstruction::SOBOL>::checkGenValue(const GenValue& genValue, const DesignParameter& designParameter)
    {
        auto dimension = genValue.first;

        if (dimension==1)
        {
            return (genValue.second.size()==1 && genValue.second.front()==0);
        }

        unsigned int degree = nthPrimitivePolynomialDegree(dimension-1);

        if (genValue.second.size() != degree){
            return false;
        }

        for(unsigned int j = 0; j < degree; ++j)
        {
              if (!(genValue.second[j] % 2 == 1)){ //each direction number is odd
                  return false;
              }
              if (!(genValue.second[j]< (unsigned int) (2<<j))){ // each direction number is small enough
                  return false;
              }
        }
        return true;
    }

    unsigned int NetConstructionTraits<NetConstruction::SOBOL>::nRows(const DesignParameter& param) { return (unsigned int) param; }

    unsigned int NetConstructionTraits<NetConstruction::SOBOL>::nCols(const DesignParameter& param) { return (unsigned int) param; }

    static const std::array<unsigned int,21200> degrees =
    {{
        #include "netbuilder/data/primitive_polynomials_degrees.csv"
    }};

    static const std::array<unsigned long,21200> representations =
    {{
        #include "netbuilder/data/primitive_polynomials_representations.csv"
    }};

    NetConstructionTraits<NetConstruction::SOBOL>::PrimitivePolynomial  NetConstructionTraits<NetConstruction::SOBOL>::nthPrimitivePolynomial(unsigned int n)
    {
        // primitive polynomials are hard-coded because their computation is really complex.
        if (n>0 && n <= 21200)
        return std::pair<unsigned int,uInteger>(degrees[n-1],representations[n-1]);
        else{
            return std::pair<unsigned int,uInteger>(0,0);
        }
    }

    unsigned int  NetConstructionTraits<NetConstruction::SOBOL>::nthPrimitivePolynomialDegree(unsigned int n)
    {
        // primitive polynomials are hard-coded because their computation is really complex.
        if (n>0 && n <= 21200)
        return degrees[n-1];
        else{
            return 0;
        }
    }

    void makeIteration(GeneratingMatrix& mat, std::list<boost::dynamic_bitset<>>& reg, const boost::dynamic_bitset<>& mask, unsigned int k)
    {
        assert(k <= mat.nCols() && k<= mat.nRows());
        boost::dynamic_bitset<> newDirNum = reg.front();
        newDirNum.resize(k);
        assert(reg.size()==mask.size());
        unsigned j = 0;
        for(boost::dynamic_bitset<> tmp : reg)
        {
            if (mask[j])
            {
                tmp.resize(k);
                tmp = tmp << (reg.size()-j);
                newDirNum ^= tmp;
            }
            ++j;
        }
        reg.pop_front();
        for(unsigned int i = 0; i < k; ++i)
        {
            mat(i, k-1) = newDirNum[k-i-1];
        }
        reg.push_back(std::move(newDirNum));
    }

    GeneratingMatrix*  NetConstructionTraits<NetConstruction::SOBOL>::createGeneratingMatrix(const GenValue& genValue, const DesignParameter& designParam, std::shared_ptr<GeneratingMatrixComputationData>& computationData)
    {
        unsigned int m  = nCols(designParam);
        unsigned int coord = genValue.first;

        if (coord==1) // special case for the first dimension
        {
            GeneratingMatrix* tmp = new GeneratingMatrix(m,m);
            for(unsigned int k = 0; k<m; ++k){
            (*tmp)(k,k) = 1; // start with identity
            }
            return tmp;
        }

        // compute the vector defining the linear recurrence on the columns of the matrix

        PrimitivePolynomial p = nthPrimitivePolynomial(coord-1);
        auto degree = p.first;
        auto poly_rep = p.second;
        boost::dynamic_bitset<> mask(degree,(poly_rep << 1) + 1);

        unsigned int matrixSize = std::max(degree,m);
        GeneratingMatrix* tmp = new GeneratingMatrix(matrixSize, matrixSize);

        std::list<boost::dynamic_bitset<>> reg;
        unsigned int k = 1;
        for(auto dirNum : genValue.second)
        {
            reg.push_back(boost::dynamic_bitset<>(k,dirNum));
            for(unsigned int i = 0; i < k; ++i)
            {
                (*tmp)(i,k-1) = reg.back()[k-i-1];
            }
            ++k;
        }

        while (k<=matrixSize)
        {
            makeIteration(*tmp,reg, mask, k);
            ++k;
        }
        computationData = std::make_shared<GeneratingMatrixComputationData>(k, std::move(mask), std::move(reg));
        return tmp;
    }

    std::vector<std::vector<uInteger>> NetConstructionTraits<NetConstruction::SOBOL>::readJoeKuoDirectionNumbers(unsigned int dimension)
    {
        assert(dimension >= 1 && dimension <= 21201);
        std::ifstream file("../share/latbuilder/data/JoeKuoSobolNets.csv");
        std::vector<std::vector<uInteger>> res(dimension);
        std::string sent;
        for(unsigned int i = 1; i <= dimension; ++i)
        {
            if(getline(file,sent))
            {
                std::vector<std::string> fields;
                boost::split( fields, sent, boost::is_any_of( ";" ) );
                for( const auto& token : fields)
                {
                    res[i-1].push_back(std::stol(token));
                }
            }
            else
            {
                break;
            }
        }
        return res;
    }

    std::vector<GenValue> NetConstructionTraits<NetConstruction::SOBOL>::defaultGenValues(unsigned int dimension, const DesignParameter& designParameter){
        std::vector<std::vector<uInteger>> tmp = readJoeKuoDirectionNumbers(dimension);
        std::vector<GenValue> res(dimension);
        for(unsigned int j = 0; j < dimension; ++j)
        {
            res[j] = GenValue(j+1,tmp[j]);
        }
        return res;
    }

    std::vector<GenValue> NetConstructionTraits<NetConstruction::SOBOL>::genValueSpaceDim(unsigned int dimension, const DesignParameter& designParameter)
    {
        unsigned int size;
        if (dimension==1)
        {
            return std::vector<GenValue>{GenValue(1,{0})};
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

    std::vector<std::vector<GenValue>> NetConstructionTraits<NetConstruction::SOBOL>::genValueSpace(unsigned int maxDimension, const DesignParameter& designParameter)
    {
        std::vector<std::vector<GenValue>> seqs;
        seqs.reserve(maxDimension);
        for(unsigned int i = 0; i < maxDimension; ++i)
        {
            seqs.push_back(genValueSpaceDim(i+1,designParameter));
        }
        LatBuilder::SeqCombiner<std::vector<GenValue>, LatBuilder::CartesianProduct> tmp(seqs);
        std::vector<std::vector<GenValue>> res;
        for(const auto& foo : tmp)
        {
            res.push_back(foo);
        }
        return res;
    }

    void NetConstructionTraits<NetConstruction::SOBOL>::extendGeneratingMatrices( 
            unsigned int nRows,
            unsigned int nCols,
            std::vector<std::shared_ptr<GeneratingMatrix>>& genMats, 
            std::vector<std::shared_ptr<GeneratingMatrixComputationData>>& computationData)
    {
        unsigned int s = (unsigned int) genMats.size();
        for(unsigned int k = 0; k < s; ++k)
        {
            if (nCols > genMats[k]->nCols())
            {
                unsigned int oldNCols = genMats[k]->nCols();
                genMats[k]->resize(nRows,nCols);
                if(k==0)
                {
                    {
                        for(unsigned int i = oldNCols; i < nCols; ++i)
                        {
                            genMats[k]->flip(i,i);
                        }
                    }
                }
                else
                {
                    unsigned int col = std::get<0>(*computationData[k]);
                    while (col<= nCols)
                    {
                        makeIteration(*genMats[k],std::get<2>(*computationData[k]),std::get<1>(*computationData[k]),col);
                        ++col;
                    }
                    std::get<0>(*computationData[k]) = col;
                }
            }
        }
    }

    std::string NetConstructionTraits<NetConstruction::SOBOL>::format(const std::vector<std::shared_ptr<GenValue>>& genVals, const DesignParameter& designParameter, OutputFormat outputFormat)
    {
        std::string res;
        res += "SobolDigitalNet(\n  Direction numbers = \n";
        for(const auto& genVal : genVals)
        {
            res+="  ";
            for(const auto& dirNum : genVal->second)
            {
                res+= std::to_string(dirNum);
                res+= " ";
            }
            res.pop_back();
            res+="\n";
        }
        res+=")";
        return res;
    }  
}


