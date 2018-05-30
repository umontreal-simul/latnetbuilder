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

void first_pivot(GeneratingMatrix M, int verbose= 0){
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

int iteration_on_k(std::vector<GeneratingMatrix>& Origin_Mats, int k, ProgressiveRowReducer rowReducer, bool verbose=false){
    int s = (int) Origin_Mats.size();
    unsigned int m = (int) Origin_Mats[0].nCols();

    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> rows_order = compositionsChanges(k, s);
    
    // Initialization of row map from original matrices to computation matrix
    std::map<std::pair<int, int>, int> Origin_to_M;
    std::map<int, std::pair<int, int>> M_to_Origin;

    for (int i=0; i<k-s+1; i++){
        Origin_to_M[{1, i+1}] = i;
        M_to_Origin[i] = {1, i+1};
        rowReducer.addRow(Origin_Mats[0].subMatrix(i, 1, m));
    }
    for (int i=1; i<s; i++){
        Origin_to_M[{i+1, 1}] = k-s+i;
        M_to_Origin[k-s+i] = {i+1, 1};
        rowReducer.addRow(Origin_Mats[i].subMatrix(0, 1, m));
    }

    if (rowReducer.computeRank() != rowReducer.m_nRows){
        // std::cout << "end after initialization" << std::endl;
        return 1;
    }
    // rowReducer.check();

    int step = 0;
    for (auto it = begin (rows_order); it != end (rows_order); ++it) {
        // if (verbose){
        //     std::cout << "STEP " << step << std::endl;
        // }
        step++;
        std::pair<std::pair<int, int>, std::pair<int, int>> rowChange = *it;

        int ind_exchange = Origin_to_M[rowChange.first];
        Origin_to_M[rowChange.second] = ind_exchange;
        Origin_to_M.erase(rowChange.first);
        M_to_Origin[ind_exchange] = rowChange.second;
        
        GeneratingMatrix newRow = Origin_Mats[rowChange.second.first-1].subMatrix(rowChange.second.second-1, 1, m);

        // if (verbose){
        //     std::cout << "new row " << newRow << std::endl;
        // }

        rowReducer.exchangeRow(ind_exchange, std::move(newRow), 0, verbose-1);
        // rowReducer.check();
        if (rowReducer.computeRank() != rowReducer.m_nRows){
            // first_pivot(rowReducer.m_OriginalMatrix);
            // std::cout << rowReducer.m_OriginalMatrix << std::endl;
            // std::cout << std::endl;
            // GeneratingMatrix gen(0, rowReducer.m_nCols);
            // for (const auto kv: M_to_Origin){
            //     gen.vstack(Origin_Mats[kv.second.first-1].subMatrix(kv.second.second-1, 1, m));
            //     // std::cout << kv.second.first << " " << kv.second.second << std::endl;
            // }
            // std::cout << std::endl;
            // std::cout << gen << std::endl;
            return 1;
        }
    }
    return 0;
}

int computeTValue(std::vector<GeneratingMatrix> Origin_Mats, int maxSubProj, int verbose=0)
{
    int nRows = (int) Origin_Mats[0].nRows();
    int nCols = (int) Origin_Mats[0].nCols();
    int s = (int) Origin_Mats.size();
    if (s == 1){    // to be modified!
        return 0;
    }

    int kMax = nRows-maxSubProj;
    ProgressiveRowReducer rowReducer = ProgressiveRowReducer();
    
    // std::vector<ProgressiveRowReducer> vecRowRed;
    // Initialization of row map from original matrices to computation matrix
    // std::vector<std::map<std::pair<int, int>, int>> vecOrigin_to_M;

    
    for (int k=kMax; k >= s; k--){
        // std::cout << "begin iteration " << k << std::endl;
        rowReducer.reset(nCols);
        int status = iteration_on_k(Origin_Mats, k, rowReducer, verbose-1);
        // int status = iteration_on_k(Origin_Mats, k, vecRowRed[k-s], vecOrigin_to_M[k-s], verbose);
        if (verbose){
            std::cout << "after iteration " << k << ", status : " << status << std::endl;
        }     
        if (status == 0){
            return std::max(nCols-k, maxSubProj);
        }
    }
    return std::max(nCols-s+1, maxSubProj);
}


int main(int argc, const char *argv[])
{        
    int m = 25;
    int s = 7;
    int nLevel=25;

    typedef NetConstructionTraits<NetConstruction::SOBOL> ConstructionMethod;

    typedef ConstructionMethod:: template RandomGenValueGenerator <LatBuilder::LFSR258> randomGenValueGenerator;
    randomGenValueGenerator randomGen = randomGenValueGenerator();

    auto net = std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(1, m);
    for (int dim=2; dim<=s; dim++){
        net = net->extendDimension(randomGen(dim));
    }

    // auto net = DigitalNetConstruction<NetConstruction::SOBOL>(s,m);
    

    std::vector<std::vector<GeneratingMatrix>> v;
    for (int level=0; level<nLevel; level++){
        std::vector<GeneratingMatrix> gen;
        // std::vector<GeneratingMatrix> v2;
        for (int i=1; i<=s; i++){
            gen.push_back(net->pointerToGeneratingMatrix(i)->subMatrix(m, m-level));
            // v2.push_back(net->pointerToGeneratingMatrix(i)->subMatrix(m, m-1));
        }
        
        // v.push_back(*(net.pointerToGeneratingMatrix(i)));
        v.push_back(gen);
    }
    

    clock_t t1,t2, t3, t4;
    float diff1, diff2, diff3;

    std::vector<int> iNew;
    std::vector<int> iNew2;
    std::vector<unsigned int> iOld; 
    std::vector<unsigned int> maxSubProj (nLevel, 0);
    for (int i=0; i<1; i++){
        t1=clock();
        for (int level=nLevel-1; level>=0; level--){
            iNew.push_back(computeTValue(v[level], 0, 0));
        }
        t2=clock();
        // for (int level=nLevel-1; level>=0; level--){
        //     iNew2.push_back(SchmidMethod::computeTValue(v[level], 0, 0));
        // }
        t3=clock();
        iOld = GaussMethod::computeTValue(v[0], m-nLevel, maxSubProj, 0);
        t4=clock();
        diff1 += ((float)t2-(float)t1);
        diff2 += ((float)t3-(float)t2);
        diff3 += ((float)t4-(float)t3);
    }
    // int res2 = computeTValue(gen, 0, 0);
    // int res3 = SchmidMethod::computeTValue(gen, 0, 0);
    // std::cout << "old gauss " << res2 << std::endl;
    // std::cout << "old schmid " << res3 << std::endl;
    // res2 = computeTValue(v2, 0, 0);
    // res3 = SchmidMethod::computeTValue(v2, 0, 0);
    // std::cout << "old gauss " << res2 << std::endl;
    // std::cout << "old schmid " << res3 << std::endl;

    // std::vector<unsigned int> maxSubProj(2, 0);
    // std::vector<int> res = GaussMethod::computeTValue(gen, m-2, maxSubProj, 1);
    
    // int iNew = GaussMethod::computeTValue(gen, 0, 0);

    std::cout << "Gauss standard s=" << s << ", m=" << m << ", time= " << diff1 <<std::endl;
    for (int level=0; level<nLevel; level++){
        std::cout << iNew[level] << " ";
    }
    std::cout << std::endl;

    // std::cout << "Schmid s=" << s << ", m=" << m << ", time= " << diff2 <<std::endl;
    // for (int level=0; level<nLevel; level++){
    //     std::cout << iNew2[level] << " ";
    // }
    // std::cout << std::endl;

    std::cout << "Gauss embedded s=" << s << ", m=" << m << ", time= " << diff3 <<std::endl;
    for (int level=0; level<nLevel; level++){
        std::cout << iOld[level] << " ";
    }
    std::cout  << std::endl;

    // std::cout << res[0] << " " << res[1] << std::endl;
    

    return 0;
}
