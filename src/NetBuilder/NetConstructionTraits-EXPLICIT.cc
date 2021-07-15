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

#include "netbuilder/NetConstructionTraits.h"

#include <sstream>
#include <boost/algorithm/string/erase.hpp>

namespace NetBuilder {

    const std::string NetConstructionTraits<NetConstruction::EXPLICIT>::name = "Explicit";
    
    typedef typename NetConstructionTraits<NetConstruction::EXPLICIT>::GenValue GenValue;

    typedef NetConstructionTraits<NetConstruction::EXPLICIT>::SizeParameter SizeParameter;

    bool NetConstructionTraits<NetConstruction::EXPLICIT>::checkGenValue(const GenValue& genValue, const SizeParameter& sizeParameter)
    {
        return genValue.nRows() == nRows(sizeParameter) && genValue.nCols() == nCols(sizeParameter);
    }

    unsigned int NetConstructionTraits<NetConstruction::EXPLICIT>::nRows(const SizeParameter& sizeParameter) {return (unsigned int) sizeParameter.first; }

    unsigned int NetConstructionTraits<NetConstruction::EXPLICIT>::nCols(const SizeParameter& sizeParameter) {return (unsigned int) sizeParameter.second; }

    GeneratingMatrix*  NetConstructionTraits<NetConstruction::EXPLICIT>::createGeneratingMatrix(const GenValue& genValue, const SizeParameter& sizeParameter)
    {
        GeneratingMatrix* genMat = new GeneratingMatrix(genValue);
        genMat->resize(nRows(sizeParameter),nCols(sizeParameter));
        return genMat;
    }

    std::vector<int>*  NetConstructionTraits<NetConstruction::EXPLICIT>::createGeneratingVector(const GenValue& genValue, const SizeParameter& sizeParameter)
    {
      unsigned int m = (unsigned int) (nCols(sizeParameter));
      std::vector<int>* cj = new std::vector<int>;
      for(unsigned int c =0; c<m; c++ ){
          cj->push_back(0);
      }
      GeneratingMatrix* tmp = createGeneratingMatrix(genValue,sizeParameter);
      for(unsigned int c =0; c<m; c++ )
      {
          for(unsigned int r =0; r<m; r++ )
          {
             (*cj)[c] += (*tmp)[r][c]*pow(2, m-1-r); 
          }
      }
        return cj;
    }

    std::vector<GenValue> NetConstructionTraits<NetConstruction::EXPLICIT>::genValueSpaceCoord(Dimension coord, const SizeParameter& sizeParameter)
    {
        throw std::logic_error("The space of all matrices is far too big to be exhautively explored.");
        return std::vector<GenValue>{};
    }

    std::vector<std::vector<GenValue>> NetConstructionTraits<NetConstruction::EXPLICIT>::genValueSpace(Dimension dimension, const SizeParameter& sizeParameter)
    {
        throw std::logic_error("The space of all matrices is far too big to be exhautively explored.");
        return std::vector<std::vector<GenValue>>{};
    }

    std::string NetConstructionTraits<NetConstruction::EXPLICIT>::format(const std::vector<std::shared_ptr<GenValue>>& genVals, const SizeParameter& sizeParameter, OutputFormat outputFormat, OutputMachineFormat outputMachineFormat, unsigned int interlacingFactor)
    {
        std::ostringstream stream;
        
        if (outputFormat == OutputFormat::HUMAN){
            stream << "Explicit Digital Net - Matrix size = " << sizeParameter.first << "x" << sizeParameter.second << std::endl;
            /*for (unsigned int coord = 0; coord < genVals.size(); coord++){
                if (interlacingFactor == 1){
                    stream << "Coordinate " << coord+1 << ":" << std::endl;
                }
                else{
                    if (coord % interlacingFactor == 0){
                        stream << "Coordinate " << (coord / interlacingFactor) + 1  << ":" << std::endl;
                    }
                    stream << "    Component " << (coord % interlacingFactor) + 1  << ":" << std::endl;
                }
                stream << *(genVals[coord]) << std::endl;
            }*/
        }
        else{
            unsigned int m = (unsigned int) (nCols(sizeParameter));
            stream << "Explicit  // Construction method" << std::endl;
            stream << "# Parameters for a digital net in base 2 " << std::endl;
            stream << genVals.size()/interlacingFactor << "   # dimensions" << std::endl;
            stream << m << "   # k = "<<m << ", n = 2^"<<  m << " = " << (int)pow(2,m ) << std::endl;
            stream << "m   # r = " << m << " digits" << std::endl;
            stream << "# The next row gives the columns of C_1 , the first gen . matrix" << std::endl;
            for(unsigned int coord = 0; coord < genVals.size(); coord++)
            {
                const std::vector<int>* generatingVector = createGeneratingVector(*(genVals[coord]), sizeParameter);
                //stream << coord +1 << " ";
                for(unsigned int i = 0; i < m; ++i)
                {
                    stream << (*generatingVector)[i] << " ";
                }
                stream << std::endl;
            }
            std::string foo(stream.str());
            foo.pop_back();
            stream.str(foo);
            stream.seekp (0, stream.end);
        }
        return stream.str();
    }  
}
