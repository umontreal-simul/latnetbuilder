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
#include "netbuilder/Util.h"

#include "latbuilder/SeqCombiner.h"
#include "latbuilder/Traversal.h"

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

        if (dimension==0)
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
        #include "../data/primitive_polynomials_degrees.csv"
    }};

    static const std::array<unsigned long,21200> representations =
    {{
        #include "../data/primitive_polynomials_representations.csv"
    }};

    NetConstructionTraits<NetConstruction::SOBOL>::PrimitivePolynomial  NetConstructionTraits<NetConstruction::SOBOL>::nthPrimitivePolynomial(Dimension n)
    {
        // primitive polynomials are hard-coded because their computation is really complex.
        if (n>0 && n <= 21200)
        return std::pair<unsigned int,uInteger>(degrees[n-1],representations[n-1]);
        else{
            return std::pair<unsigned int,uInteger>(0,0);
        }
    }

    unsigned int  NetConstructionTraits<NetConstruction::SOBOL>::nthPrimitivePolynomialDegree(Dimension n)
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
        Dimension coord = genValue.first;

        if (coord==0) // special case for the first dimension
        {
            GeneratingMatrix* tmp = new GeneratingMatrix(m,m);
            for(unsigned int k = 0; k<m; ++k){
            (*tmp)(k,k) = 1; // start with identity
            }
            return tmp;
        }

        // compute the vector defining the linear recurrence on the columns of the matrix
        PrimitivePolynomial p = nthPrimitivePolynomial(coord);
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
            makeIteration(*tmp, reg, mask, k);
            ++k;
        }
        computationData = std::make_shared<GeneratingMatrixComputationData>(k, std::move(mask), std::move(reg));
        return tmp;
    }

    
    const char* ws = " \t\n\r\f\v";

    // trim from end (right)
    inline std::string& rtrim(std::string& s, const char* t = ws)
    {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from beginning (left)
    inline std::string& ltrim(std::string& s, const char* t = ws)
    {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from both ends (left & right)
    inline std::string& trim(std::string& s, const char* t = ws)
    {
        return ltrim(rtrim(s, t), t);
    }    

    std::vector<std::vector<uInteger>> NetConstructionTraits<NetConstruction::SOBOL>::readJoeKuoDirectionNumbers(Dimension dimension)
    {
        assert(dimension >= 1 && dimension <= 21201);
        std::ifstream file("../share/latnetbuilder/data/JoeKuoSobolNets.csv");
        std::vector<std::vector<uInteger>> res(dimension);
        std::string sent;

        do
        {
            getline(file,sent);
            trim(sent);
        }
        while (sent != "###");

        getline(file,sent);

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

    std::vector<GenValue> NetConstructionTraits<NetConstruction::SOBOL>::defaultGenValues(Dimension dimension, const DesignParameter& designParameter){
        std::vector<std::vector<uInteger>> tmp = readJoeKuoDirectionNumbers(dimension);
        std::vector<GenValue> res(dimension);
        for(unsigned int j = 0; j < dimension; ++j)
        {
            res[j] = GenValue(j,tmp[j]);
        }
        return res;
    }

   NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::GenValueSpaceCoordSeq(Dimension coord):
    m_coord(coord),
    m_underlyingSeq(underlyingSeqs(coord))
    {};


    NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::const_iterator::const_iterator(const GenValueSpaceCoordSeq& seq):
        m_coord(seq.coord()),
        m_underlyingIterator(seq.underlyingSeq().begin()),
        m_value(GenValue(m_coord, *m_underlyingIterator))
    {};

    NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::const_iterator::const_iterator(const GenValueSpaceCoordSeq& seq, end_tag):
        m_coord(seq.coord()),
        m_underlyingIterator(seq.underlyingSeq().end())
    {};

    bool NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::const_iterator::equal(const const_iterator& other) const
    { 
        return m_underlyingIterator == other.m_underlyingIterator;
    }

    const NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::const_iterator::value_type& NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::const_iterator::dereference() const
    { 
        return m_value;
    }

    void NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::const_iterator::increment()
    {
        if (m_underlyingIterator != m_underlyingIterator.seq().end())
        {
            ++m_underlyingIterator;
            m_value = GenValue(m_coord, *m_underlyingIterator);
        }
    }

    NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::const_iterator NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::begin() const
    {
        return const_iterator(*this);
    }

    NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::const_iterator NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::end() const
    {
        return const_iterator(*this, typename const_iterator::end_tag{} );
    }

    Dimension NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::coord() const
    {
        return m_coord;
    }

    size_t  NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::size() const
    {
        return m_underlyingSeq.size();
    }

    const LatBuilder::SeqCombiner<std::vector<uInteger>,LatBuilder::CartesianProduct>& NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::underlyingSeq() const
    {
        return m_underlyingSeq;
    }

    std::vector<std::vector<uInteger>> NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq::underlyingSeqs(Dimension coord)
    {
        if (coord == 0)
        {
            return {{0}};
        }
        else
        {
            unsigned int size = nthPrimitivePolynomialDegree(coord);
            std::vector<uInteger> dirNums{1};
            dirNums.reserve( (1 << (size - 1))  - 1);

            std::vector<std::vector<uInteger>> seqs;
            seqs.reserve(size);
            seqs.push_back(dirNums);

            uInteger upperBound = 2;
            for(unsigned int i = 1; i < size; ++i)
            {
                upperBound = 2 * upperBound;
                for(uInteger k = dirNums.back() + 2; k < upperBound; k += 2)
                {
                    dirNums.push_back(k);
                }
                seqs.push_back(dirNums);
            }
            return seqs;
        }
    }

    typename NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceCoordSeq NetConstructionTraits<NetConstruction::SOBOL>::genValueSpaceCoord(Dimension coord, const DesignParameter& designParameter)
    {
        return GenValueSpaceCoordSeq(coord);
    }

    typename NetConstructionTraits<NetConstruction::SOBOL>::GenValueSpaceSeq NetConstructionTraits<NetConstruction::SOBOL>::genValueSpace(Dimension dimension, const DesignParameter& designParameter)
    {
        std::vector<GenValueSpaceCoordSeq> seqs;
        seqs.reserve(dimension);
        for(Dimension coord = 0; coord < dimension; ++coord)
        {
            seqs.push_back(genValueSpaceCoord(coord, designParameter));
        }
        return GenValueSpaceSeq(seqs);
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


