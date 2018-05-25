// // This file is part of Lattice Builder.
// //
// // Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.

#include <vector>
#include <math.h>
#include <iostream>

typedef std::vector<bool> Polynomial;
typedef std::vector<std::vector<bool>> Matrix;

unsigned int getmsb (unsigned long long x){
    unsigned int r = 0;
    if (x < 1) return 0;
    while (x >>= 1) r++;
    return r;
}

int deg(const Polynomial& p){
    return p.size()-1;
}

bool coeff(const Polynomial& p, int i){
    if (i < p.size()){
        return p[i];
    }
    return 0;
}

int numPoints(const Matrix& matrix){
    return pow(2, matrix.size());
}

std::vector<unsigned long> getColsReverse(const Matrix& matrix){
    std::vector<unsigned long> res(matrix.size(), 0);
    for (unsigned int j=0; j<matrix.size(); j++){
        unsigned long s = 0;
        for (unsigned int i=0; i<matrix.size(); i++){
            s += matrix[i][j] << (matrix.size() - i -1);
        }
        res[j] = s;
    }
    return res;
}


void expandSeries(const Polynomial& h, const Polynomial& P, std::vector<unsigned int>& expansion, unsigned int expansion_limit){
    int m = deg(P); 
    for(int l = 1; l<= expansion_limit ; l++){
        int res =  (m-l >=0 && coeff(h, m-l))? 1 : 0;
        int start = (l-m > 1) ? (l-m) : 1;
        for( int p = start; p < l; p++){
            res = ( res + expansion[p-1] * coeff(P, m-(l-p))) %2;        
        }
        expansion[l-1] = res;
    }
   }


std::vector<Matrix> computeGeneratorMatrices(const Polynomial& modulus, const std::vector<Polynomial>& gen) {
        
    unsigned int m = (unsigned int) (deg(modulus));

    std::vector<Matrix> genratorMatrices(gen.size(), std::vector<std::vector<bool>>(m, std::vector<bool>(m, 0)));

    for(unsigned int j=0; j<gen.size(); j++){
    
        std::vector<unsigned int> expansion(2 * m);
        expandSeries(gen[j], modulus, expansion, 2 * m);

        for(unsigned int c =0; c<m; c++ ){
            unsigned int res = 0;
            // unsigned int mult = 1;
            for(unsigned int row =0; row <m; row++ ){
                genratorMatrices[j][row][c] = expansion[c + row];
                // res += expansion[c + row] * mult;
                // mult *= 2;
            }
            // genratorMatrices[j].push_back(res);
        }
    }
    return genratorMatrices;
   }


std::vector<std::vector<double>> points(std::vector<Matrix> matrices, std::vector<unsigned int> graycode){
    std::vector<std::vector<double>> points;
    int nbPoints = numPoints(matrices[0]);
    for (int i=0; i<nbPoints; i++){
        points.push_back(std::vector<double>());
    }

    for (int j=0; j < matrices.size(); j++){
        Matrix matrix = matrices[j];
        std::vector<unsigned long> cols = getColsReverse(matrix);
        std::vector<unsigned long> m_permutation = std::vector<unsigned long>(nbPoints, 0);
        points[0].push_back(0);
        for (unsigned int i=1; i<m_permutation.size(); ++i){
            m_permutation[i] = m_permutation[i-1] ^ cols[graycode[i-1]];
            points[i].push_back(((double) m_permutation[i]) / nbPoints);
        }
    }
    return points;
}

int main(){
    Polynomial modulus {0,1,0,1,0,1,0,1,1};
    std::vector<Polynomial> genVector {{1}, {1, 0, 0, 0, 1, 0, 1, 1}, {1, 1, 0, 1, 1, 1, 0, 1}};
    
    std::vector<Matrix> matrices = computeGeneratorMatrices(modulus, genVector);
    for (int k=0; k<matrices.size(); k++){
        for (int i=0; i<matrices[k].size(); i++){
            for (int j=0; j<matrices[k][i].size(); j++){
                std::cout << matrices[k][i][j] << " ";
            }
        std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::vector<unsigned int> graycode = std::vector<unsigned int>();
    for(unsigned int i=0; i < pow(2, deg(modulus)); i++){
          graycode.push_back(getmsb(((i >> 1) ^ i)^(((i+1) >> 1) ^ (i+1))));
    }

    std::vector<std::vector<double>> p = points(matrices, graycode);
    for (int i=0; i<p.size(); i++){
        for (int j=0; j<p[i].size(); j++){
            std::cout << p[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}