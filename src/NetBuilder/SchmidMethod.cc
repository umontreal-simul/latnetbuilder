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

#include <list>

#include "netbuilder/Util.h"

namespace NetBuilder {

int
getmsb (unsigned long long x)
{
    int r = 0;
    if (x < 1) return 0;
    while (x >>= 1) r++;
    return r;
}

typedef GeneratingMatrix GeneratingMatrix;

unsigned int SchmidMethod::computeTValue(std::vector<GeneratingMatrix> matrices, unsigned int maxTValuesSubProj, int verbose=0)
{
    unsigned int m = matrices[0].nCols();
    unsigned int s = (unsigned int)matrices.size();

    if (s==1){ return 0; } 

    uInteger upperLimit = (1<<(m-maxTValuesSubProj))-1;
    std::vector<unsigned int> flipingOrder(upperLimit);
    for(uInteger r = 0; r < upperLimit; ++r)
    {
        flipingOrder[r] = getmsb(((r >> 1) ^ r)^(((r+1) >> 1) ^ (r+1)));
    }

    for(unsigned int k = s ; k <= m-maxTValuesSubProj; ++k)
    {
        CompositionMaker compMaker(k,s);
        do
        { 
            std::vector<unsigned int> comp = compMaker.currentComposition();
            std::vector<GeneratingMatrix::Row*> tmp(k);
            unsigned int idx = 0;
            for(Dimension coord = 0; coord < s; ++coord)
            {
                for(int j = 0; j < comp[coord]; ++j)
                {
                    tmp[idx] = &(matrices[coord][j]);
                    ++idx;
                }
            }
            boost::dynamic_bitset<> v(m);
            for(uInteger r = 0; r < (unsigned int) ((1 << k) - 1); ++r)
            {
                v ^= *tmp[flipingOrder[r]];
                if (v.none())
                {
                    return m-(k-1);
                }
            }
        }
        while(compMaker.goToNextComposition());
    }
    return maxTValuesSubProj;
}

std::vector<unsigned int> SchmidMethod::computeTValue(std::vector<GeneratingMatrix> matrices, const std::vector<unsigned int>& maxTValuesSubProj, int verbose=0)
{
    unsigned int m = matrices[0].nCols();
    unsigned int s = (unsigned int)matrices.size();

    if (s==1){ return std::vector<unsigned int>(m, 0); } 

    uInteger upperLimit ;
    std::vector<unsigned int> res = maxTValuesSubProj;

    upperLimit= (1<<(m-maxTValuesSubProj.back()))-1;

    std::list<unsigned int> flipingOrder;
    for(uInteger r = 0; r < upperLimit; ++r)
    {
        flipingOrder.push_back(getmsb(((r >> 1) ^ r)^(((r+1) >> 1) ^ (r+1))));
    }

    unsigned int nextToCompute = s-1;
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
                for(int j = 0; j < comp[coord]; ++j)
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