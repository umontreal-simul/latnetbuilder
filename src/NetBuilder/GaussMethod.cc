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


namespace NetBuilder {

void first_pivot2(GeneratingMatrix M, int verbose= 0){
    int k = M.nRows();
    int m = M.nCols();
    
    int i_pivot=0;
    int j=-1;
    int Pivots[k];
    for (int i=0; i<k; i++){
        Pivots[i] = -1;
    }
    
    while (i_pivot < k && j < m-1){
        j++;
        int i_temp = i_pivot;
        while (i_temp < k && M[i_temp][j] == 0){
            i_temp++;
        }
        if (i_temp >= k){  // pas d'element non nul sur la colonne
            continue;
        }
        M.swap_rows(i_temp, i_pivot);

        Pivots[i_pivot] = j;
        for (int i=i_pivot+1; i<k; i++){
            if (M[i][j] != 0){
                M[i] = M[i] ^ M[i_pivot];
            }
        }
        i_pivot++;
    }
    if (Pivots[k-1] == -1){
        // assert(false);
        std::cout << ("not invertible") << std::endl;
    }
}

unsigned int iteration_on_k(std::vector<GeneratingMatrix>& Origin_Mats, int k, ProgressiveRowReducer& rowReducer, int mMin, int mMax, int nbCol, int verbose){
    // int ok = 0;
    int s = (int) Origin_Mats.size();

    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> rows_order = compositionsChanges(k, s);
    
    // Initialization of row map from original matrices to computation matrix
    std::map<std::pair<int, int>, int> Origin_to_M;

    for (int i=0; i<k-s+1; i++){
        Origin_to_M[{1, i+1}] = i;
        rowReducer.addRow(Origin_Mats[s-1].subMatrix(i, 1, mMax+1));
    }
    for (int i=1; i<s; i++){
        Origin_to_M[{i+1, 1}] = k-s+i;
        rowReducer.addRow(Origin_Mats[s-1-i].subMatrix(0, 1, mMax+1));
    }
    // std::cout << rowReducer.m_OriginalMatrix << std::endl;
    // std::cout << std::endl;
    // std::cout << rowReducer.m_mat << std::endl;
    // first_pivot2(rowReducer.m_OriginalMatrix.subMatrix(7, 7));


    // bool stop = rowReducer.updateStatus(mMin, status, k, ok);
    unsigned int smallestInvertible = rowReducer.computeSmallestInvertible(mMin, nbCol, k);
    // std::cout << "after initialization, smallest invertible= " << smallestInvertible << std::endl;

    if (smallestInvertible == mMax+1){
        // std::cout << "end in initialization" << std::endl;
        return mMax+1;
    }
    // if (rowReducer.computeRank() != k){
    //     throw std::runtime_error("should be full rank");
    // }

    // int step = 0;
    for (auto it = begin (rows_order); it != end (rows_order); ++it) {
        // if (verbose > 0){
        //     std::cout << "STEP " << step << std::endl;
        // }
        // step++;
        std::pair<std::pair<int, int>, std::pair<int, int>> rowChange = *it;

        int ind_exchange = Origin_to_M[rowChange.first];
        Origin_to_M[rowChange.second] = ind_exchange;
        Origin_to_M.erase(rowChange.first);
        
        GeneratingMatrix newRow = Origin_Mats[s-rowChange.second.first].subMatrix(rowChange.second.second-1, 1, mMax+1);

        // if (verbose > 0){
        //     std::cout << "new row " << newRow << std::endl;
        // }

        smallestInvertible = rowReducer.exchangeRow(ind_exchange, std::move(newRow), smallestInvertible, verbose-1);
        // if (temp < smallestInvertible){
        //     throw std::runtime_error("smallest invertible should be non decreasing");
        // }
        // smallestInvertible = temp;
        // rowReducer.check();
        // if (pivotRemoved > 0 && pivotRemoved <= smallestInvertible){
        //     std::cout << "pivot removed: " << pivotRemoved << std::endl;
        //     smallestInvertible = pivotRemoved-1;
        // }
        // stop = rowReducer.updateStatus(mMin, status, k, ok);

        if (smallestInvertible == mMax+1){
            return smallestInvertible;
        }
    }
    return smallestInvertible;
}

unsigned int GaussMethod::computeTValue(std::vector<GeneratingMatrix> Origin_Mats, unsigned int maxSubProj, int verbose=0)
{
    return GaussMethod::computeTValue(Origin_Mats, Origin_Mats[0].nCols()-1, {maxSubProj}, verbose)[0];
}

std::vector<unsigned int> GaussMethod::computeTValue(std::vector<GeneratingMatrix> Origin_Mats, int mMin, std::vector<unsigned int> maxSubProj, int verbose=0)
{
    // int m = (int) Origin_Mats[0].nCols();
    int nRows = (int) Origin_Mats[0].nRows();
    int nCols = (int) Origin_Mats[0].nCols();
    int mMax = nCols-1;
    int s = (int) Origin_Mats.size();
    unsigned int nLevel = maxSubProj.size();
    ProgressiveRowReducer rowReducer = ProgressiveRowReducer();

    // reversing the matrices to put the new matrices at the front: heuristic to gain time
    // std::reverse(std::begin(Origin_Mats), std::end(Origin_Mats));   // TODO: better?

    
    std::vector<unsigned int> result = maxSubProj;
    int diff = 0;
    if (mMin < s-1){     // TODO
        diff = (s-1-mMin);
        nLevel -= (s-1-mMin);
        mMin = s-1;
    }
    for (unsigned int i=0; i<nLevel; i++){
        result[i+diff] = std::max(nCols-(nLevel-1-i)-s+1, maxSubProj[i+diff]);
    }
    unsigned int previousIndSmallestInvertible = nLevel;
    // std::vector<int> status(nLevel, 1);
    
    if (s == 1){    // does not make sense when s == 1
        return result;
    }
    for (int k=nRows-maxSubProj[maxSubProj.size()-1]; k >= s; k--){
        // std::cout << "begin iteration " << k << std::endl;
        rowReducer.reset(mMax+1);
        unsigned int smallestInvertible = iteration_on_k(Origin_Mats, k, rowReducer, mMin, mMax, nLevel, verbose-1);
        // if (verbose){
            // std::cout << "after iteration " << k << ", smallestInvertible : " << smallestInvertible << std::endl;
        // }
        // mMax = mMin-1;
        if (smallestInvertible == mMax+1){
            continue;
        }
        // if (smallestInvertible >= mMin){
        for (unsigned int i= ((smallestInvertible > mMin) ? smallestInvertible-mMin: 0); i<previousIndSmallestInvertible; i++){
            // if (previousIndSmallestInvertible+diff > result.size()){
            //     throw std::runtime_error("oups 42");
            // }
            // std::cout << "i " << i << std::endl;
            result[i+diff] = std::max(nCols-(nLevel-1-i)-k, maxSubProj[i+diff]);
        }
        if (smallestInvertible <= mMin){
            break;
        } 
        previousIndSmallestInvertible = smallestInvertible-mMin;
            // mMax = smallestInvertible;
            // std::cout << mMax << std::endl;
        
        
        // }       
    }
    // std::cout << "end of compute tvalue" << std::endl;
    // std::cout << result.size() << std::endl;
    // std::cout << result[0] << std::endl;
    return result;
}

}
 

