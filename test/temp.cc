
#include <vector>
#include <math.h>
#include <iostream>

typedef std::vector<bool> Polynomial;
typedef std::vector<std::vector<bool>> Matrix;

std::vector<Matrix> matrices =  {  {  { 1, 0, 0 } ,  { 0, 1, 0 } ,  { 0, 0, 1 }  } ,  {  { 1, 1, 1 } ,  { 1, 1, 0 } ,  { 1, 0, 0 }  }  } ;
unsigned int m = matrices[0].size();


unsigned int getmsb (unsigned long long x){
    unsigned int r = 0;
    if (x < 1) return 0;
    while (x >>= 1) r++;
    return r;
}

int deg(const Polynomial& p){
    return p.size()-1;
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

std::vector<unsigned int> graycode = std::vector<unsigned int>();


int main(){


std::vector<unsigned int> graycode = std::vector<unsigned int>();
for(unsigned int i=0; i < pow(2, m); i++){
        graycode.push_back(getmsb(((i >> 1) ^ i)^(((i+1) >> 1) ^ (i+1))));
}

std::vector<std::vector<double>> myPoints = points(matrices, graycode);
    for (int i=0; i<myPoints.size(); i++){
        for (int j=0; j<myPoints[i].size(); j++){
            std::cout << myPoints[i][j] << " ";
        }
        std::cout << std::endl;
    }
}