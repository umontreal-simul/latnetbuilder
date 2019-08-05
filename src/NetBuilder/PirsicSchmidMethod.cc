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

#include "netbuilder/TValueComputation.h"
#include "netbuilder/Types.h"
#include "netbuilder/CompositionMaker.h"
#include "netbuilder/ProgressiveRowReducer.h"

#include <list>

#include "netbuilder/Util.h"

namespace NetBuilder {


unsigned int PirsicSchmidMethod::computeTValue(std::vector<GeneratingMatrix> matrices, unsigned int maxTValuesSubProj, int verbose=0)
{
    unsigned int m = matrices[0].nCols();
    unsigned int s = (unsigned int)matrices.size();

    if (s==1){ return 0; } 
    
    int rho = m;
    ProgressiveRowReducer rowReducer(m);
    for(unsigned int k = 1 ; k <= m; ++k)
    {
        CompositionMaker compMaker(k,s);
        do
        { 
            std::vector<unsigned int> comp = compMaker.currentComposition();
            unsigned int idx = 0;
            for(Dimension coord = 0; coord < s; ++coord)
            {
                for(unsigned int j = 0; j < comp[coord]; ++j)
                {
                    rowReducer.addRow(matrices[coord][j]);
                    ++idx;
                }
            }
            if (rowReducer.computeRank() < m){
                rho = k - 1;
                break;
            }
            rowReducer.reset(m);
        }
        while(compMaker.goToNextComposition());
    }
    return m - rho;
}

std::vector<unsigned int> PirsicSchmidMethod::computeTValue(std::vector<GeneratingMatrix> matrices, const std::vector<unsigned int>& maxTValuesSubProj, int verbose=0)
{
    unsigned int m = matrices[0].nCols();
    unsigned int s = (unsigned int)matrices.size();

    if (s==1){ return std::vector<unsigned int>(m, 0); } 
        ProgressiveRowReducer rowReducer(m);

    for(unsigned int k = 1 ; k <= m; ++k)
    {
        CompositionMaker compMaker(k,s);
        do
        { 
            std::vector<unsigned int> comp = compMaker.currentComposition();
            unsigned int idx = 0;
            for(Dimension coord = 0; coord < s; ++coord)
            {
                for(unsigned int j = 0; j < comp[coord]; ++j)
                {
                    rowReducer.addRow(matrices[coord][j]);
                    ++idx;
                }
            }
            auto ranks = rowReducer.computeRanks()
            for 
            if (rowReducer.computeRank() < m){
                rho = k - 1;
                break;
            }
            rowReducer.reset(m);
        }
        while(compMaker.goToNextComposition());
    }
    std::vector<unsigned int> res =  {0, 0 , 0, ...}


    for(unsigned int k = s ; k <= m-maxTValuesSubProj.back(); ++k)
    {
        CompositionMaker compMaker(k, s);
        do
        {
            std::vector<unsigned int> comp = compMaker.currentComposition();
            std::vector<GeneratingMatrix::Row*> tmp(k);
            unsigned int idx = 0;
            for(Dimension coord = 0; coord < s; ++coord)
            {
                for(unsigned int j = 0; j < comp[coord]; ++j)
                {
                    tmp[idx] = &(matrices[coord][j]);
                    ++idx;
                }
            }

            boost::dynamic_bitset<> v(m);
            unsigned int r = 0;
            unsigned int currentLimit = (unsigned int) ((1 << k) - 1);
            for(unsigned int flip : flipingOrder)
            {
                v ^= *tmp[flip];

                auto numberOfZeros = v.find_first();
                
                if (numberOfZeros == GeneratingMatrix::Row::npos)
                {
                    numberOfZeros = m;
                }

                for(unsigned int i = nextToCompute; i < numberOfZeros; ++i)
                {
                    res[i] = std::max(i+1-(k-1), res[i]);
                }

                if (nextToCompute < (unsigned int) numberOfZeros)
                {
                    nextToCompute = (unsigned int) numberOfZeros;
                }

                if (nextToCompute == m)
                {
                    return res;
                }

                ++r;
                if(r >= currentLimit)
                {
                    break;
                }
            }
        }
        while(compMaker.goToNextComposition());
    }
    return res;
}


}