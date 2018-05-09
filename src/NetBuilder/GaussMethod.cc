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
#include "netbuilder/TValueComputation.h"
#include "netbuilder/Util.h"

namespace NetBuilder {

void first_pivot(GeneratingMatrix& M, GeneratingMatrix& D, std::vector<int>& C, bool verbose=false){
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
        D.swap_rows(i_temp, i_pivot);
        Pivots[i_pivot] = j;
        for (int i=i_pivot+1; i<k; i++){
            if (M[i][j] != 0){
                M[i] = M[i] ^ M[i_pivot];
                D[i] = D[i] ^ D[i_pivot];
            }
        }
        i_pivot++;
    }
    if (verbose){
        std::cout << "middle of pivoting" << std::endl;
        std::cout << M << std::endl;
    }
    if (Pivots[k-1] == -1){
        throw std::runtime_error("Matrice non inversible");
    }
    
    for (int i_pivot = k-1; i_pivot >=0; i_pivot--){
        int j = Pivots[i_pivot];
        for (int i=0; i<i_pivot; i++){
            if (M[i][j] == 1){
                M[i] = M[i] ^ M[i_pivot];
                D[i] = D[i] ^ D[i_pivot];
            }
        }
    }
        
    for (int i_pivot = 0; i_pivot < k; i_pivot++){
        int j = Pivots[i_pivot];
        M.swap_columns(i_pivot, j);
        std::swap(C[i_pivot],C[j]);
        // swap_col(M, C, i_pivot, j);
    }
    if (verbose){
        std::cout << "end of pivoting" << std::endl;
        std::cout << M << std::endl;
    }  
    // delete Pivots;
}

void add_line(GeneratingMatrix& T, GeneratingMatrix& D, std::map<int, int>& Origin_to_M, std::vector<int>& C, Row& newLine, int i_new_in_Origin[2], int i_old_in_Origin[2], bool verbose=false){
    int k = T.nRows();
    int m = T.nCols();
    
    int i_old=-1;
    int i_old_inM = Origin_to_M[i_old_in_Origin[0]*m + i_old_in_Origin[1]];
    if (verbose){
        std::cout << T << std::endl;
        std::cout << D << std::endl;
        std::cout << "i_old_inM = j_old : " << i_old_inM << std::endl;
    }    
    int j_old = i_old_inM;
    bool found = false;
    
    for (int i=0; i<k; i++){
        if (D[i][j_old] == 1){
            if (! found){
                i_old = i;
                found = true;
            }
            else{
                T[i] = T[i] ^ T[i_old];
                D[i] = D[i] ^ D[i_old];
            }
        }
    }
    if (verbose){
        std::cout << "i_old : " << i_old << std::endl;
        std::cout << "de-pivot old line" << std::endl;
        std::cout << T << std::endl;
        std::cout << D << std::endl;
    }
    
    T[i_old] = newLine;
    D[i_old] = Row(k, 0);
    for (int i=0; i<k; i++){
        D[i][j_old] = 0;
    }
    D[i_old][j_old] = 1;
    if (verbose){
        std::cout << "swapped old line and new line" << std::endl;
        std::cout << T << std::endl;
        std::cout << D << std::endl;
    }
    
    Origin_to_M.erase(i_old_in_Origin[0]*m + i_old_in_Origin[1]);
    Origin_to_M[i_new_in_Origin[0]*m + i_new_in_Origin[1]] = i_old_inM;
    
    for (int i=0; i<k; i++){
        if (i != i_old && T[i_old][i] != 0){
            T[i_old] = T[i] ^ T[i_old];
            D[i_old] = D[i] ^ D[i_old];
        }
    }
    if (verbose){
        std::cout << "simplified new line" << std::endl;
        std::cout << T << std::endl;
        std::cout << D << std::endl;
    }

    if (T[i_old][i_old] == 0){
        int j = k;
        while (j < m and T[i_old][j] == 0){
            j += 1;
        }
        if (j == m){
            throw std::runtime_error("Nouvelle matrice non inversible");
        }
        else{
            T.swap_columns(i_old, j);
            std::swap(C[i_old],C[j]);
            // swap_col(T, C, i_old, j);
        }
    }
    if (verbose){
        std::cout << "brought new pivot on the good column" << std::endl;
        std::cout << T << std::endl;
        std::cout << D << std::endl;
    }
    
    for (int i=0; i<k; i++){
        if (i == i_old){
            continue;
        }
        if (T[i][i_old] == 1){
            T[i] = T[i] ^ T[i_old];
            // D[i] = (Row) D[i] ^ D[i_old];
            D[i] = D[i] ^ D[i_old];
        }
    }
    if (verbose){
        std::cout << "re-pivoted with new pivot" << std::endl;
        std::cout << T << std::endl;
        std::cout << D << std::endl;
    }
}

int iteration_on_k(std::vector<GeneratingMatrix>& Origin_Mats, int k, bool verbose=false){
    int s = Origin_Mats.size();
    int m = Origin_Mats[0].nCols();

    std::vector<std::vector<int>> lines_order = compositions(k, s);
    
    // Initialization of data structures
    std::map<int, int> Origin_to_M;
    GeneratingMatrix T(k, m);
    GeneratingMatrix D(k, m);
    for (int i=0; i<k-s+1; i++){
        Origin_to_M[i] = i;
        T[i] = Row(Origin_Mats[0][i]);
    }
    for (int i=1; i<s; i++){
        Origin_to_M[i*m] = i;
        T[k-s+i] = Row(Origin_Mats[i][0]);
    }
    for (int i=0; i<k; i++){
        D[i] = Row(k, 0);
        D[i][i] = 1;
    }
    std::vector<int> C;
    for (int j=0; j<m; j++){
        C.push_back(j);
    }

    try{
        first_pivot(T, D, C, verbose);
    }
    catch (const std::runtime_error& e){
        return 1;
    }

    int step = 0;
    for (auto it = begin (lines_order); it != end (lines_order) - 1; ++it) {
        if (verbose){
            std::cout << "STEP " << step << std::endl;
        }
        step++;
        std::vector<int> old_choice_of_lines = *it;
        std::vector<int> new_choice_of_lines = *(it+1);
        Row new_line;
        int ind_new_line[2];
        int ind_old_line[2];

        for (int i=0; i<s; i++){
            if (new_choice_of_lines[i] - old_choice_of_lines[i] == 1){
                // indice of new matrix = i
                // because of numbering conventions, indice of new line in matrix = old_choice_of_lines[i] = new_choice_of_lines[i] - 1
                ind_new_line[0] = i; 
                ind_new_line[1] = old_choice_of_lines[i];
                new_line = Row(Origin_Mats[i][old_choice_of_lines[i]]);
            }
            if (new_choice_of_lines[i] - old_choice_of_lines[i] == -1){
                // indice of new matrix = i
                // because of numbering conventions, indice of old line in matrix = new_choice_of_lines[i] = old_choice_of_lines[i] - 1
                ind_old_line[0] = i; 
                ind_old_line[1] = new_choice_of_lines[i];
            }
        }
        if (verbose){
            std::cout << "new line " << new_line << std::endl;
        }

        // apply permutation to new line
        new_line = permutation(new_line, C);

        try{
            add_line(T, D, Origin_to_M, C, new_line, ind_new_line, ind_old_line, verbose);
        }
        catch (const std::runtime_error& e){
            return 1;
        }
    }
    return 0;
}

int GaussMethod::computeTValue(std::vector<GeneratingMatrix> Origin_Mats, int maxSubProj, bool verbose=false)
{
    int m = Origin_Mats[0].nRows();
    int s = Origin_Mats.size();
    if (s == 1){    // to be modified!
        return 0;
    }
    for (int k=m-maxSubProj; k >= s; k--){
        int status = iteration_on_k(Origin_Mats, k, verbose);
        if (verbose){
            std::cout << "after iteration " << k << ", status : " << status << std::endl;
        }     
        if (status == 0){
            return std::max(m-k, maxSubProj);
        }
    }
    return std::max(m-s+1, maxSubProj);
}

}


