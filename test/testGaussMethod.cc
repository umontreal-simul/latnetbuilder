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

#include "latbuilder/DigitalNet/DigitalUtil.h"
#include "latbuilder/DigitalNet/SchmidMethod.h"
#include "latbuilder/DigitalNet/GaussMethod.h"
#include "latbuilder/DigitalNet/DigitalNet.h"
#include "latbuilder/DigitalNet/SobolNet.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#include <stdlib.h>


using namespace LatBuilder::DigitalNet;

typedef LatBuilder::uInteger uInteger;

int main(int argc, const char *argv[])
{
    std::cout << "constructing matrices" << std::endl;

    // std::vector<std::string> S1 = {"10000", "01000", "00100", "00010", "00001"};
    // std::vector<std::string> S2 = {"00001", "00010", "00100", "01000", "10000"};
    // Matrix<2> M1(5, 5, S1);
    // Matrix<2> M2(5, 5, S2);
    // std::vector<Matrix<2>> foo = {M2, M1};
    // int m = 6;
    
    // std::vector<std::string*> S1;
    // std::string* str;
    // for (int i=0; i<m; i++){
    //     str = new std::string[m];
    //     // str = "";
    //     for (int j=0; j<m; j++){
    //         *(str + j) = (char) rand() % 2;
    //     }
    //     // int v1 = 
    //     S1.push_back(str);
    // }
    // std::cout<< *(S1[0]) <<std::endl;
    // Matrix<2> M1(m, m, S1);
    // std::cout<<M1<<std::endl;

    // std::vector<std::string> S1 = {"111001", "010011", "001010", "000110", "100010", "010100"};
    // std::vector<std::string> S2 = {"101011", "011101", "001000", "000101", "010100", "010001"};
    // std::vector<std::string> S3 = {"100100", "010000", "001000", "000100", "111001", "010011"};
    // Matrix<2> M1(6, 6, S1);
    // Matrix<2> M2(6, 6, S2);
    // Matrix<2> M3(6, 6, S3);  
    // std::vector<Matrix<2>> foo = {M1, M2, M3};  

    // std::vector<std::vector<uInteger>> directionNumbers = {{}, {1},{1,3},{1,3,1}, 
    //                                                 {1,1,1},{1,1,3,3}, {1, 3, 5, 13}, {1, 1, 5, 5, 17}, {1, 1, 5, 5, 5}};
    std::vector<std::vector<uInteger>> directionNumbers = {{}, {1},{1,3},{1,3,1}};

    int m = 400;
    int s = 4;
    std::vector<Matrix<2>> foo = SobolNet(m,s,directionNumbers).generatingMatrices();
    foo.erase(foo.begin());

    clock_t t1,t2, t3, t4;   
    // clock_t t3, t4;    

    // t1=clock();
    // std::cout << "Schmid method" << std::endl;
    // std::cout << SchmidMethod::computeTValue(foo,0) << std::endl;
    // t2=clock();
    // float diff ((float)t2-(float)t1);
    // cout<<diff<<endl;

    t3=clock();
    std::cout << "Gauss method" << std::endl;
    bool verbose = false;
    std::cout << GaussMethod::computeTValue(foo,0, verbose) << std::endl;
    t4=clock();
    float diff2 ((float)t4-(float)t3);
    cout<<diff2<<endl;

    return 0;
}
