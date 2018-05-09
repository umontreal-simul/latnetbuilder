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

// #include "netbuilder/Types.h"
// #include "netbuilder/GeneratingMatrix.h"
// #include "netbuilder/NetConstructionTraits.h"

// #include <vector>
// #include <assert.h>

// namespace NetBuilder {

//     typedef typename NetConstructionTraits<NetConstruction::UNIRANDOM>::GenValue GenValue;

//     bool NetConstructionTraits<NetConstruction::UNIRANDOM>::checkGenValue(const GenValue& genValue)
//     {
//         return true;
//     }

//     GeneratingMatrix*  NetConstructionTraits<NetConstruction::UNIRANDOM>::createGeneratingMatrix(const GenValue& genValue, unsigned int nRows, unsigned int nCols) 
//     {
//         return new GeneratingMatrix(nRows, nCols, true);
//     }

//     std::vector<GenValue> NetConstructionTraits<NetConstruction::UNIRANDOM>::defaultGenValues(unsigned int dimension){
//         std::vector<GenValue> res(dimension);
//         for(unsigned int j = 0; j < dimension; ++j)
//         {
//             res[j] = j;
//         }
//         return res;
//     }

//     void NetConstructionTraits<NetConstruction::UNIRANDOM>::extendGeneratingMatrices( 
//         unsigned int inc,
//         const std::vector<std::shared_ptr<GeneratingMatrix>>& genMats, 
//         const std::vector<std::shared_ptr<GenValue>>& genValues)
//     {
//         unsigned int s = genMats.size();
//         for(unsigned int k = 0; k < s; ++k)
//         {
//             unsigned int nCols = genMats[k]->nCols();
//             unsigned int nRows = genMats[k]->nRows();

//             genMats[k]->resize(nRows+inc,nCols+inc);
//             for(unsigned int i = nRows; i < nRows+inc; ++i)
//             {
//                 (*genMats[k])(i,i).flip();
//                 for(unsigned int j = i+1; j < nCols+inc; ++j )
//                 {
//                     (*genMats[k])(i,j) = rand() % 2;
//                 }
//             }
//         }
//     }
// }

