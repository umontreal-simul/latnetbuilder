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

#include "netbuilder/TValueComputation.h"
#include "netbuilder/Types.h"
#include "netbuilder/CompositionMaker.h"

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

    size_type upperLimit = (1<<(m-maxTValuesSubProj))-1;
    std::vector<unsigned int> flipingOrder(upperLimit);
    for(uInteger r = 0; r < upperLimit; ++r)
    {
        flipingOrder[r] = getmsb(((r >> 1) ^ r)^(((r+1) >> 1) ^ (r+1)));
    }

    unsigned int k;
    for(k = s ; k <= m-maxTValuesSubProj; ++k)
    {
        CompositionMaker compMaker(k,s);
        do
        { 
            std::vector<unsigned int> comp = compMaker.currentComposition();
            std::vector<GeneratingMatrix::Row*> tmp(k);
            unsigned int idx = 0;
            for(unsigned int coord = 0; coord < s; ++coord)
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
                    return m-k+1;
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

    std::cout << s << std::endl;

    if (s==1){ return std::vector<unsigned int>(m); } 

    size_type upperLimit ;
    std::vector<unsigned int> res;
    if (maxTValuesSubProj.size()>=1)
    {
        upperLimit= (1<<(m-maxTValuesSubProj.back()))-1;
        res = maxTValuesSubProj;
    }
    else
    {
        upperLimit = (1<<m)-1;
        res.resize(m,0);
    }
    std::vector<unsigned int> flipingOrder(upperLimit);
    for(uInteger r = 0; r < upperLimit; ++r)
    {
        flipingOrder[r] = getmsb(((r >> 1) ^ r)^(((r+1) >> 1) ^ (r+1)));
    }

    unsigned int alreadyComputed = 0;
    unsigned int k;
    for(k = s ; k <= m-maxTValuesSubProj.back(); ++k)
    {
        std::cout << k << std::endl;
        CompositionMaker compMaker(k, s);
        do
        {
            std::vector<unsigned int> comp = compMaker.currentComposition();
            std::cout << k << std::endl;
            std::vector<GeneratingMatrix::Row*> tmp(k);
            unsigned int idx = 0;
            for(unsigned int coord = 0; coord < s; ++coord)
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
                while(!v[alreadyComputed] && alreadyComputed < m)
                {
                    res[alreadyComputed] = std::max(alreadyComputed+2-k, maxTValuesSubProj[alreadyComputed]);
                    ++alreadyComputed;
                }
                if (alreadyComputed == m)
                {
                    return res;
                }
            }
        }
        while(compMaker.goToNextComposition());
    }
    return res;
}


}