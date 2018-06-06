// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
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

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/ProgressiveRowReducer.h"

#include "netbuilder/FigureOfMerit/UniformityProperties.h"
#include <iostream>

using namespace NetBuilder;

int main(int argc, const char *argv[])
{
    unsigned int s = 3;
    unsigned int m = 7;

    auto net = DigitalNetConstruction<NetConstruction::SOBOL>(s,m);

    auto rowReducer = ProgressiveRowReducer();

    std::cout << "Setting the number of columns." << std::endl;
    rowReducer.reset(m);
    std::cout << "Done." << std::endl;

    // std::vector<unsigned long> seeds(m);
    // for(unsigned int i = 0; i < m; ++i)
    // {
    //     seeds[i] = rand() % (1 << m);
    // }
    // GeneratingMatrix mat(m, m, seeds);

    // std::cout << mat << std::endl;

    for(unsigned int i = 0; i < 5; ++i)
    {
        auto newRow = net.pointerToGeneratingMatrix(0)->subMatrix(i,0,1,m);
        std::cout << newRow << std::endl;
        rowReducer.addRow(newRow);
    }

        // rowReducer.addRow(mat.subMatrix(i,1,m));

        std::cout << "Matrix: " << std::endl;
        std::cout << rowReducer.matrix() << std::endl;

        std::cout << "Row operations: " << std::endl;
        std::cout << rowReducer.rowOperations() << std::endl;

        std::cout << "Columns without pivot: " << std::endl;
        for (unsigned int col : rowReducer.m_columnsWithoutPivot)
        {
            std::cout << col << " ";
        }
        std::cout << std::endl;

        std::cout << "Rows without pivot: " << std::endl;
        for (unsigned int row : rowReducer.m_rowsWithoutPivot)
        {
            std::cout << row << " ";
        }
        std::cout << std::endl;

        std::cout << "Pivot positions" << std::endl;
        for (const auto& colRow : rowReducer.m_pivotsColRowPositions)
        {
            std::cout << colRow.second << ", " << colRow.first << " | ";
        }
        std::cout << std::endl << std::endl;
    }

    // std::cout << "Pivoted matrix" << std::endl;
    // std::cout << rowReducer.matrix() << std::endl;

    // std::cout << "Check with row operations" << std::endl;
    // std::cout << rowReducer.rowOperations() * mat << std::endl;

    std::cout << "Rank of full matrix: " << rowReducer.computeRank() << std::endl;

    std::cout << "Rank of sub matrices: " << std::endl;
    for(unsigned int rank : rowReducer.computeRanks(1,m-1))
    {
        std::cout << rank << " ";
    }
    std::cout << std::endl;

    std::vector<unsigned long> newColSeeds(m,0);
    for(unsigned int i = 0; i < m; ++i)
    {
        newColSeeds[i] = 1;
    }

    std::cout << std::endl <<  "================" <<  std::endl << std::endl;

    GeneratingMatrix dummyRow = GeneratingMatrix(1,m);
    for(unsigned int i = 0; i < m; ++i)
    {
        dummyRow(0,i) = 1;
    }
    rowReducer.replaceRow(1,dummyRow);


    std::cout << std::endl <<  "================" <<  std::endl << std::endl;

    // GeneratingMatrix newCol(m, 1, newColSeeds);

    // rowReducer.addColumn(newCol);

    std::cout << "Matrix: " << std::endl;
    std::cout << rowReducer.matrix() << std::endl;

    std::cout << "Row operations: " << std::endl;
    std::cout << rowReducer.rowOperations() << std::endl;

    std::cout << "Columns without pivot: " << std::endl;
    for (unsigned int col : rowReducer.m_columnsWithoutPivot)
    {
        std::cout << col << " ";
    }
    std::cout << std::endl;

    std::cout << "Rows without pivot: " << std::endl;
    for (unsigned int row : rowReducer.m_rowsWithoutPivot)
    {
        std::cout << row << " ";
    }
    std::cout << std::endl;

    std::cout << "Pivot positions" << std::endl;
    for (const auto& colRow : rowReducer.m_pivotsColRowPositions)
    {
        std::cout << colRow.second << ", " << colRow.first << " | ";
    }
    std::cout << std::endl;
    for (const auto& colRow : rowReducer.m_pivotsRowColPositions)
    {
        std::cout << colRow.first << ", " << colRow.second << " | ";
    }
    std::cout << std::endl << std::endl;
    
    return 0;
}
