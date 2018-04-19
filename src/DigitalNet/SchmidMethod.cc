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

#include "latbuilder/DigitalNet/SchmidMethod.h"
#include "latbuilder/DigitalNet/DigitalUtil.h"

using namespace LatBuilder::DigitalNet;

int
getmsb (unsigned long long x)
{
    int r = 0;
    if (x < 1) return 0;
    while (x >>= 1) r++;
    return r;
}

typedef Matrix<2> GeneratingMatrix;

int SchmidMethod::computeTValue(std::vector<GeneratingMatrix> matrices, unsigned int maxTValuesSubProj)
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
        std::vector<std::vector<int>> comps  = compositions(k,s);
        for (const auto& comp : comps)
        {   
            std::vector<GeneratingMatrix::Row*> tmp(k);
            unsigned int idx = 0;
            for(unsigned int coord = 0; coord < s; ++coord)
            {
                for(unsigned int j = 0; j < comp[coord]; ++j)
                {
                    tmp[idx] = &(matrices[coord][j]);
                    ++idx;
                }
            }

            boost::dynamic_bitset<> v(m);
            for(uInteger r = 0; r < ((1 << k) - 1); ++r)
            {
                v ^= *tmp[flipingOrder[r]];
                if (v.none())
                {
                    return m-k+1;
                }
            }
        }
    }
    return maxTValuesSubProj;
}

int ReversedSchmidMethod::computeTValue(std::vector<GeneratingMatrix> matrices, unsigned int maxTValuesSubProj)
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

    unsigned int k = m-maxTValuesSubProj;
    bool isFullRank = false;
    while(k>=s && !isFullRank)
    {
        std::vector<std::vector<int>> comps  = compositions(k,s);
        bool isFullRankSubSystem = true;
        int idxComp = 0;

        do
        {
            std::vector<GeneratingMatrix::Row*> tmp(k);
            unsigned int idx = 0;
            for(unsigned int coord = 0; coord < s; ++coord)
            {
                for(unsigned int j = 0; j < comps[idxComp][coord]; ++j)
                {
                    tmp[idx] = &(matrices[coord][j]);
                    ++idx;
                }
            }

            boost::dynamic_bitset<> v(m);
            
            for(uInteger r = 0; r < ((1 << k) - 1); ++r)
            {
                v ^= *tmp[flipingOrder.at(r)];
                if (v.none())
                {
                    isFullRankSubSystem = false;
                    break;
                }
            }
            idxComp++;
        }
        while(isFullRankSubSystem && idxComp < comps.size());
        isFullRank = isFullRankSubSystem;
        --k;
    }
    //assert(isFullRank);
    //std::cout << k << std::endl;
    return std::max(m-(k+1),maxTValuesSubProj);
}