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

#include <map>
#include <fstream>
#include <algorithm>

#include "netbuilder/TValueComputation.h"
#include "netbuilder/Util.h"
#include "netbuilder/ProgressiveRowReducer.h"
#include "netbuilder/CompositionMaker.h"



namespace NetBuilder {

unsigned int iteration_on_k(std::vector<GeneratingMatrix>& baseMatrices, unsigned int k, unsigned int mMin, int verbose){
    unsigned int nCols = baseMatrices[0].nCols();
    unsigned int s = (unsigned int) baseMatrices.size();
    
    // Initialization of row map from original matrices to computation matrix
    std::map<std::pair<int, int>, int> Origin_to_M;

    ProgressiveRowReducer rowReducer(nCols);

    for (unsigned int i=0; i<k-s+1; i++){
        Origin_to_M[{1, i+1}] = i;
        rowReducer.addRow(baseMatrices[s-1].subMatrix(i, 0, 1, nCols));
    }
    for (unsigned int i=1; i<s; i++){
        Origin_to_M[{i+1, 1}] = k-s+i;
        rowReducer.addRow(baseMatrices[s-1-i].subMatrix(0, 0, 1, nCols));
    }

    unsigned int smallestFullRankIndex = rowReducer.smallestFullRank() - 1;
    if (verbose > 0){
        std::cout << "after initialization, smallest invertible= " << smallestFullRankIndex << std::endl;
    }

    if (smallestFullRankIndex == nCols){
        if (verbose > 0){
            std::cout << "end in initialization" << std::endl;
        }
        return nCols;
    }

    CompositionMaker compositionMaker(k, s);

    while (compositionMaker.goToNextComposition()) {

        std::pair<std::pair<int, int>, std::pair<int, int>> rowChange = compositionMaker.changeFromPreviousComposition();

        int ind_exchange = Origin_to_M[rowChange.first];
        Origin_to_M[rowChange.second] = ind_exchange;
        Origin_to_M.erase(rowChange.first);
        
        GeneratingMatrix newRow = baseMatrices[s-rowChange.second.first].subMatrix(rowChange.second.second-1, 0, 1, nCols);

        rowReducer.replaceRow(ind_exchange, std::move(newRow), verbose-1);

        smallestFullRankIndex = rowReducer.smallestFullRank() - 1;

        if (smallestFullRankIndex == nCols){
            return smallestFullRankIndex;
        }
    }
    return smallestFullRankIndex;
}

unsigned int GaussMethod::computeTValue(std::vector<GeneratingMatrix> baseMatrices, unsigned int maxSubProj, int verbose=0)
{
    unsigned int s = (unsigned int) baseMatrices.size();
    if (s == 1)
    {
        return 0;
    }

    return GaussMethod::computeTValue(baseMatrices, baseMatrices[0].nCols()-1, {maxSubProj}, verbose)[0];
}

std::vector<unsigned int> GaussMethod::computeTValue(std::vector<GeneratingMatrix> baseMatrices, unsigned int mMin, const std::vector<unsigned int>& maxSubProj, int verbose=0)
{
    unsigned int nRows = baseMatrices[0].nRows();
    unsigned int nCols = baseMatrices[0].nCols();
    unsigned int s = (unsigned int) baseMatrices.size();

    unsigned int nLevel = (unsigned int) maxSubProj.size();
    ProgressiveRowReducer rowReducer = ProgressiveRowReducer();
    
    if (s == 1){    // does not make sense when s == 1
        return std::vector<unsigned int>(nCols-mMin);
    }

    std::vector<unsigned int> result = maxSubProj;
    unsigned int diff = 0;
    if (mMin < s-1){
        diff = (s-1-mMin);
        nLevel -= (s-1-mMin);
        mMin = s-1;
    }
    for (unsigned int i=0; i<nLevel; i++){
        result[i+diff] = std::max(nCols-(nLevel-1-i)-s+1, maxSubProj[i+diff]);
    }
    unsigned int previousIndSmallestInvertible = nLevel;
    

    for (unsigned int k=nRows-maxSubProj[maxSubProj.size()-1]; k >= s; k--){
        if (verbose > 0){
            std::cout << "begin iteration " << k << std::endl;
        }
        unsigned int smallestFullRankIndex = iteration_on_k(baseMatrices, k, mMin, verbose-1);
        if (verbose > 0){
            std::cout << "after iteration " << k << ", smallestFullRankIndex : " << smallestFullRankIndex << std::endl;
        }
        if (smallestFullRankIndex == nCols){
            continue;
        }
        for (unsigned int i= ((smallestFullRankIndex > mMin) ? smallestFullRankIndex-mMin: 0); i<previousIndSmallestInvertible; i++){
            if (verbose > 0){
                std::cout << "storing t-value of index " << i << std::endl;
            }
            result[i+diff] = std::max(nCols-(nLevel-1-i)-k, maxSubProj[i+diff]);
        }
        if (smallestFullRankIndex <= mMin){
            break;
        } 
        previousIndSmallestInvertible = smallestFullRankIndex-mMin;
    
    }
    return result;
}

}
 

