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
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/ProgressiveRowReducer.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/TValueComputation.h"
#include <time.h>
#include <algorithm>
#include <random>
#include <chrono>




#include "latbuilder/LFSR258.h"

using namespace NetBuilder;

int iteration_on_k(std::vector<GeneratingMatrix>& Origin_Mats, int k, ProgressiveRowReducer rowReducer, bool verbose=false){
    int s = (int) Origin_Mats.size();
    int m = (int) Origin_Mats[0].nCols();
    rowReducer.reset(m);

    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> rows_order = compositionsChanges(k, s);
    
    int lim = (int) rows_order.size()/5;
    // std::cout << lim << std::endl;
    std::vector<int> state (s, 1);
    state[0] = k-s+1;
    for (int i=0; i<lim; i++){
        state[rows_order[i].first.first-1]--;
        state[rows_order[i].second.first-1]++;
    }
    // for (int i=0; i<s; i++){
    //     std::cout << state[i] << " ";
    // }
    // std::cout << std::endl;
    
    std::map<std::pair<int, int>, int> Origin_to_M;
    int count = 0;
    for (int i=0; i<s; i++){
        for (int j=0; j< state[i]; j++){
            Origin_to_M[{i+1, j+1}] = count;
            rowReducer.addRow(Origin_Mats[i].subMatrix(j, 1, m));
            count++;
        }
    }

    ProgressiveRowReducer rowReducer2 = rowReducer;
    std::map<std::pair<int, int>, int> Origin_to_M2 = Origin_to_M;

    // std::cout << rowReducer.computeRank() << std::endl;
    // std::cout << k << std::endl;
    // std::cout << rowReducer.m_nRows << std::endl;
    if (rowReducer.computeRank() != k){
        // std::cout << "not good nb of rows" << std::endl;
        return 1;
    }

    // rowReducer.reset(m);
    // std::map<std::pair<int, int>, int> Origin_to_M2;
    // for (int i=1; i<s; i++){
    //     Origin_to_M2[{i+1, 1}] = i-1;
    //     rowReducer.addRow(Origin_Mats[i].subMatrix(0, 1, m));
    // }

    // for (int i=0; i<k-s+1; i++){
    //     Origin_to_M2[{1, i+1}] = i+s-1;
    //     rowReducer.addRow(Origin_Mats[0].subMatrix(i, 1, m));
    // }


    // if (rowReducer.computeRank() != k){
    //     return 1;
    // }

    

    // rowReducer.check();

    int step = 0;
    for (int i = lim; i<rows_order.size(); i++){
    // for (auto it = begin (rows_order); it != end (rows_order); ++it) {
        if (verbose){
            std::cout << "STEP " << step << std::endl;
        }
        step++;
        // std::pair<std::pair<int, int>, std::pair<int, int>> rowChange = *it;
        std::pair<std::pair<int, int>, std::pair<int, int>> rowChange = rows_order[i];
        auto p = rowChange;

        int ind_exchange = Origin_to_M[rowChange.first];
        Origin_to_M[rowChange.second] = ind_exchange;
        Origin_to_M.erase(rowChange.first);

        GeneratingMatrix newRow = Origin_Mats[rowChange.second.first-1].subMatrix(rowChange.second.second-1, 1, m);       

        if (verbose){
            std::cout << "new row " << newRow << std::endl;
            std::cout << "position " << ind_exchange << std::endl;
        }

        rowReducer.exchangeRow(ind_exchange, newRow, 1);
        // rowReducer.check();
        if (rowReducer.computeRank() != rowReducer.m_nRows){
            return 1;
        }
    }

    // rowReducer.reset(m);
    // std::map<std::pair<int, int>, int> Origin_to_M2;
    // for (int i=1; i<s; i++){
    //     Origin_to_M2[{i+1, 1}] = i-1;
    //     rowReducer.addRow(Origin_Mats[i].subMatrix(0, 1, m));
    // }

    // for (int i=0; i<k-s+1; i++){
    //     Origin_to_M2[{1, i+1}] = i+s-1;
    //     rowReducer.addRow(Origin_Mats[0].subMatrix(i, 1, m));
    // }

    // if (rowReducer.computeRank() != k){
    //     return 1;
    // }

    step = 0;
    for (int i = 0; i<lim; i++){
    // for (auto it = begin (rows_order); it != end (rows_order); ++it) {
        if (verbose){
            std::cout << "STEP " << step << std::endl;
        }
        step++;
        // std::pair<std::pair<int, int>, std::pair<int, int>> rowChange = *it;
        std::pair<std::pair<int, int>, std::pair<int, int>> rowChange = rows_order[lim-1-i];
        auto p = rowChange;

        int ind_exchange = Origin_to_M2[rowChange.second];
        Origin_to_M2[rowChange.first] = ind_exchange;
        Origin_to_M2.erase(rowChange.second);

        GeneratingMatrix newRow = Origin_Mats[rowChange.first.first-1].subMatrix(rowChange.first.second-1, 1, m);       

        if (verbose){
            std::cout << "new row " << newRow << std::endl;
            std::cout << "position " << ind_exchange << std::endl;
        }

        rowReducer2.exchangeRow(ind_exchange, newRow, 1);
        // rowReducer.check();
        if (rowReducer2.computeRank() != rowReducer.m_nRows){
            return 1;
        }
    }



    return 0;
}

int computeTValue(std::vector<GeneratingMatrix> Origin_Mats, int maxSubProj, int verbose=0)
{
    int m = (int) Origin_Mats[0].nRows();
    int s = (int) Origin_Mats.size();
    if (s == 1){    // to be modified!
        return 0;
    }

    int kMax = m-maxSubProj;
    ProgressiveRowReducer rowReducer = ProgressiveRowReducer();
    
    // std::vector<ProgressiveRowReducer> vecRowRed;
    // Initialization of row map from original matrices to computation matrix
    // std::vector<std::map<std::pair<int, int>, int>> vecOrigin_to_M;

    
    for (int k=kMax; k >= s; k--){
        int status = iteration_on_k(Origin_Mats, k, rowReducer, verbose);
        // int status = iteration_on_k(Origin_Mats, k, vecRowRed[k-s], vecOrigin_to_M[k-s], verbose);
        if (verbose){
            std::cout << "after iteration " << k << ", status : " << status << std::endl;
        }     
        if (status == 0){
            return std::max(m-k, maxSubProj);
        }
    }
    return std::max(m-s+1, maxSubProj);
}


int main(int argc, const char *argv[])
{        
    int m = 30;
    int s = 3;

    typedef NetConstructionTraits<NetConstruction::SOBOL> ConstructionMethod;

    typedef ConstructionMethod:: template RandomGenValueGenerator <LatBuilder::LFSR258> randomGenValueGenerator;
    randomGenValueGenerator randomGen = randomGenValueGenerator();

    auto net = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(1, m);
    for (int dim=2; dim<=s; dim++){
        net = net->extendDimension(randomGen(dim));
    }

    // auto net = DigitalNetConstruction<NetConstruction::SOBOL>(s,m);
    
    

    std::vector<GeneratingMatrix> v;
    for (int i=1; i<=s; i++){
        v.push_back(*(net->pointerToGeneratingMatrix(i)));
        // v.push_back(*(net.pointerToGeneratingMatrix(i)));
    }

    
    std::vector<GeneratingMatrix> v2 = v;
    // for (int i=1; i<=s; i++){
    //     v2.push_back(*(net->pointerToGeneratingMatrix((i % s) +1 )));
    //     // v.push_back(*(net.pointerToGeneratingMatrix(i)));
    // }


    clock_t t1,t2, t3;
    float diff1, diff2;

    t1=clock();
    // int iNew = computeTValue(v, 0, 0);
    // for (int i=0; i<10; i++){
    //     int iOld = GaussMethod::computeTValue(v, 0, 0);
    // }
    // int iOld = GaussMethod::computeTValue(v, 0, 0);
    for (int i=0; i<10000; i++){
        t1=clock();
        // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        // std::minstd_rand0 generator (seed);
        // std::shuffle(std::begin(v2), std::end(v2), generator);
        int iNew = computeTValue(v, 0, 0);
        t2=clock();
        int iOld = GaussMethod::computeTValue(v, 0, 0);
        t3=clock();
        diff1 += ((float)t2-(float)t1);
        diff2 += ((float)t3-(float)t2);
    }
    int iNew = computeTValue(v, 0, 0);
    int iOld = GaussMethod::computeTValue(v, 0, 0);

    std::cout << "s=" << s << ", m=" << m << ", value=" << iNew << ", time= " << diff1 <<std::endl;
    std::cout << "s=" << s << ", m=" << m << ", value=" << iOld << ", time= " << diff2 <<std::endl;

    return 0;
}
