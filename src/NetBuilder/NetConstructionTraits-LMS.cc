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
#include "netbuilder/JoeKuo.h"

#include <sstream>
#include <boost/algorithm/string/erase.hpp>


namespace NetBuilder {

    using JoeKuo::readJoeKuoDirectionNumbers;

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

    unsigned int NetConstructionTraits<NetConstruction::LMS>::nRows(const SizeParameter& sizeParameter) {return (unsigned int) sizeParameter.first; }

    unsigned int NetConstructionTraits<NetConstruction::LMS>::nCols(const SizeParameter& sizeParameter) {return (unsigned int) sizeParameter.second; }

    static const std::array<unsigned int,21200> degrees =
    {{
        #include "../data/primitive_polynomials_degrees.csv"
    }};

    static const std::array<unsigned long,21200> representations =
    {{
        #include "../data/primitive_polynomials_representations.csv"
    }};

     std::pair<unsigned int,uInteger> SobolPrimitivePolynomial(Dimension n)
    {
        // primitive polynomials are hard-coded because their computation is really complex.
        if (n>0 && n <= 21200)
        return std::pair<unsigned int,uInteger>(degrees[n-1],representations[n-1]);
        else{
            return std::pair<unsigned int,uInteger>(0,0);
        }
    }

    /** Compute the element-wise product of two vector and reduce the resulting vector using the exclusive or operator.
     * @param a first vector
     * @param b second vector 
     */ 
    unsigned int xor_prod_reduce(const std::vector<unsigned int>& a, const std::vector<unsigned int>& b)
    {
        unsigned int res = 0;
        unsigned int n = a.size();
        for (unsigned int i = 0; i<n; ++i){
            res ^= a[i]*b[i];
        }
        return res;
    }

    /** Compute the m-bit binary representation of the given integer. The most significant bit is the leftest non zero
     * bit in the returned vector.
     * @param num non-negative integer
     * @param unsigned int size of the binary representation
     */
    std::vector<unsigned int> bin_vector(unsigned int num, unsigned int m)
    {
        std::vector<unsigned int> res(m);
        for(unsigned int i = 0; i<m; ++i){
            res[m-i-1] = num % 2;
            num = num >> 1;
        }
        return res;
    }

    BinaryMatrix createGeneratingSobolMatrix(const DirectionNumber& directionNumber, unsigned int m, unsigned int coord) 
    {
        std::vector<std::vector<bool>> tmp (m, std::vector<bool>(m, 0));

        for(unsigned int k = 0; k<m; ++k){
            tmp[k][k] = 1; // start with identity
        }

        if (coord==1) // special case for the first dimension
        {
            return tmp;
        }

        // compute the vector defining the linear recurrence on the columns of the matrix

        PrimitivePolynomial p = SobolPrimitivePolynomial(coord-1);
        auto degree = p.first;
        auto poly_rep = p.second;

        std::vector<unsigned int> a = bin_vector(poly_rep,degree-1);
        a.push_back(1);

        for(unsigned int i = 0; i<degree; ++i){
            a[i] *= 2 << i;
        }

        // initialization of the first columns

        for(unsigned int k = 0; k < std::min(degree,m); ++k){
            auto dirNum = bin_vector(directionNumber[k],k+1);

            for(unsigned int i = 0; i<k; ++i){
                tmp[i][k] = dirNum[i];
            }
        }

        if (m > degree)
        {
            std::vector<unsigned int> reg(degree); // register for the linear reccurence
            std::reverse_copy(directionNumber.begin(),directionNumber.end(), reg.begin()); // should be reversed

            // computation of the recurrence
            for(unsigned int k = degree; k<m; ++k){
                unsigned int new_num = xor_prod_reduce(a,reg) ^ reg[degree-1];
                reg.pop_back();
                reg.insert(reg.begin(),new_num);
                auto dirNum = bin_vector(new_num,k+1);
                for(unsigned int i = 0; i<k; ++i){
                    tmp[i][k] = dirNum[i];
                }
            }
        }

        return tmp;
    }

    GeneratingMatrix*  NetConstructionTraits<NetConstruction::LMS>::createGeneratingMatrix(const GenValue& genValue, const SizeParameter& sizeParameter, const Dimension& dimension_j)
    {
        unsigned int m = (unsigned int) (nCols(sizeParameter));
        unsigned int w = (unsigned int) (nCols(sizeParameter));
        GeneratingMatrix* lmsMat = new GeneratingMatrix(m,m);
        std::vector<unsigned int> L(w,0);
        std::vector<unsigned int> C(m,0);
        std::vector<unsigned int> S(m,0);

        GeneratingMatrix* genMat = new GeneratingMatrix(genValue);
        genMat->resize(nRows(sizeParameter),nCols(sizeParameter));
        std::vector<std::vector<uInteger>> JoeKuo_table = readJoeKuoDirectionNumbers(dimension_j+1);
        std::vector<uInteger> directionNumbers_j = JoeKuo_table[dimension_j];
        BinaryMatrix sobol = createGeneratingSobolMatrix(directionNumbers_j, m, dimension_j+1);

        // Computation of C
        for(unsigned int c =0; c<m; c++ )
        {
            for(unsigned int r =0; r<m; r++ )
            {
                L[c] += (*genMat)(c,r) * pow(2, w-1-r); // transpose of genvalue
                C[c] += sobol[r][c] * pow(2, w-1-r);
            }
        }
        // Computation of S
        //unsigned int pow2wm1 = 1<<(w-1); //2^(w-1)
        for(unsigned int c =0; c<w; c++ )
            {
                //L[c] = (pow2wm1 + std::rand()%(pow2wm1 + 1)) >> c ;
                for(unsigned int l =0; l<m; l++ )
                {
                    S[l] ^= ((C[l] >> (w-c-1) ) & 1 )* L[c];
                }
            }
        // Computation of lmsMat
        for(unsigned int c =0; c<m; c++ )
        {   std::bitset< 31 > bits = std::bitset< 31>(S[c]);
            for(unsigned int row =0; row <m; row++ )
            {
                (*lmsMat)(row,c) = bits[m-1-row] ;
            }
        }

        return lmsMat;
    }

    std::vector<int>*  NetConstructionTraits<NetConstruction::LMS>::createGeneratingVector(const GenValue& genValue, const SizeParameter& sizeParameter, const Dimension& dimension_j)
    {
      unsigned int m = (unsigned int) (nCols(sizeParameter));
      std::vector<int>* cj = new std::vector<int>;
      for(unsigned int c =0; c<m; c++ ){
          cj->push_back(0);
      }
      GeneratingMatrix* tmp = createGeneratingMatrix(genValue,sizeParameter,dimension_j);
      std::vector<std::vector<int>> genMat (31, std::vector<int> (m, 0));
      for(unsigned int c =0; c<m; c++ )
      {
          for(unsigned int r =0; r<m; r++ )
          {
             genMat[r][c]=(*tmp)[r][c];
          }
          for(unsigned int r =0; r<31; r++ )
          {
             (*cj)[c] += genMat[r][c]*pow(2, 31-1-r); 
          }
      }
        return cj;
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

    std::string NetConstructionTraits<NetConstruction::LMS>::format(const std::vector<std::shared_ptr<GenValue>>& genVals, const SizeParameter& sizeParameter, OutputFormat outputFormat, OutputStyle outputStyle, unsigned int interlacingFactor)
    {
        std::ostringstream stream;
        
        if (outputFormat == OutputFormat::HUMAN){
            stream << "LMS Digital Net - Matrix size = " << sizeParameter.first << "x" << sizeParameter.second << std::endl;
        }
        else{
            unsigned int m = (unsigned int) (nCols(sizeParameter));
            stream << "# Parameters for a digital net in base 2" << std::endl;
            stream << genVals.size()/interlacingFactor << "    # " <<  genVals.size()/interlacingFactor << " dimensions" << std::endl;
            stream << m << "   # k = "<< m << ", n = 2^"<<  m << " = " << (int)pow(2,m ) << " points"<< std::endl;
            stream << 31 << "   # r = 31 digits" << std::endl;
            stream << "# The next row gives the columns of C_1, the first gen. matrix" << std::endl;
            for(unsigned int coord = 0; coord < genVals.size(); coord++)
            {
                const std::vector<int>* generatingVector = createGeneratingVector(*(genVals[coord]), sizeParameter, coord);
                for(unsigned int i = 0; i < m; ++i)
                {
                    stream << (*generatingVector)[i] << " ";
                }
                if(coord < genVals.size() - 1){stream << std::endl;}
            }
        }
        return stream.str();
    }  
}
