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

#ifndef NET_BUILDER__DIGITAL_NET_H
#define NET_BUILDER__DIGITAL_NET_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/GeneratingMatrix.h"
#include "netbuilder/NetConstructionTraits.h"
#include <memory>

namespace NetBuilder {

typedef size_t size_type;

class DigitalNet
{
    public:

        virtual ~DigitalNet() = default;

        DigitalNet(unsigned int dimension, unsigned int nRows, unsigned int nCols):
            m_dimension(dimension),
            m_nRows(nRows),
            m_nCols(nCols)
        {};

        /** Return the number of columns of generating matrices of the net. */
        unsigned int numColumns() const { return m_nCols; }

        /** Returns the modulus of the net*/
        unsigned int numRows() const { return m_nRows; };

        /** Returns the number of points of the net */
        uInteger numPoints() const { return intPow(2, m_nCols) ; }

        /** Returns the number of points of the net */
        uInteger size() const { return intPow(2, m_nCols) ; }

        /** Returns the dimension (number of coordinates) of the net.*/
        unsigned int dimension() const { return m_dimension ; }

        /** Returns the generating matrix of the net corresponding to the given coordinate.
            * @param coord an integer constant refering to the coordinate
        */
        GeneratingMatrix generatingMatrix(unsigned int coord) const 
        {
            return m_generatingMatrices[coord-1]->subMatrix(m_nRows, m_nCols);
        }

        GeneratingMatrix generatingMatrix(unsigned int coord, unsigned int nRows, unsigned int nCols)
        {
            return m_generatingMatrices[coord-1]->subMatrix(nRows, nCols);
        }

    protected:
        std::vector<std::shared_ptr<GeneratingMatrix>> m_generatingMatrices;
        unsigned int m_dimension;
        unsigned int m_nRows;
        unsigned int m_nCols;

        DigitalNet(unsigned int dimension, unsigned int nRows, unsigned int nCols, std::vector<std::shared_ptr<GeneratingMatrix>> genMatrices):
            m_dimension(dimension),
            m_nRows(nRows),
            m_nCols(nCols),
            m_generatingMatrices(genMatrices)
        {};

};

template <NetConstruction NC>
class DigitalNetBase : public DigitalNet
{
    public:

        typedef NetConstructionTraits<NC> ConstructionMethod;

        typedef typename ConstructionMethod::GenValue GenValue;

        DigitalNetBase():
            DigitalNet(0,0,0)
        {};

        DigitalNetBase(unsigned int nRows, unsigned int nCols):
            DigitalNet(0, nRows, nCols)
        {};

        DigitalNetBase(unsigned int m):
            DigitalNet(0, m, m)
        {};


        DigitalNetBase(unsigned int dimension, unsigned int nRows, unsigned int nCols):
            DigitalNet(dimension, nRows, nCols)
        {
            std::vector<GenValue> genValues = ConstructionMethod::defaultGenValues(dimension);
            for(const auto& genValue : genValues)
            {
                m_generatingMatrices.push_back(std::shared_ptr<GeneratingMatrix>(ConstructionMethod::createGeneratingMatrix(genValue,m_nRows,m_nCols)));
                m_genValues.push_back(std::shared_ptr<GenValue>(new GenValue(genValue)));
            }
        };

        ~DigitalNetBase() = default;

        DigitalNetBase<NC> extendDimension(){
            std::vector<GenValue> genValues = ConstructionMethod::defaultGenValues(m_dimension+1);
            std::shared_ptr<GeneratingMatrix> newMat(ConstructionMethod::createGeneratingMatrix(genValues[m_dimension],m_nRows,m_nCols));

            auto genMats = m_generatingMatrices; 
            genMats.push_back(std::move(newMat));

            auto genVals = m_genValues;
            genVals.push_back(std::shared_ptr<GenValue>(new GenValue(genValues[m_dimension])));

            return DigitalNetBase(std::move(genMats), std::move(genVals), m_dimension+1, m_nRows, m_nCols);
        }

        DigitalNetBase<NC> extendDimension(const GenValue& newGenValue){
            std::shared_ptr<GeneratingMatrix> newMat(ConstructionMethod::createGeneratingMatrix(newGenValue,m_nRows,m_nCols));

            auto genMats = m_generatingMatrices; 
            genMats.push_back(std::move(newMat));

            auto genVals = m_genValues;
            genVals.push_back(std::shared_ptr<GenValue>(new GenValue(newGenValue)));

            return DigitalNetBase(std::move(genMats), std::move(genVals), m_dimension+1, m_nRows, m_nCols);
        }

        DigitalNetBase<NC> extendSize(unsigned int inc)
        {
            ConstructionMethod::extendGeneratingMatrices(inc, m_generatingMatrices, m_genValues);
            return DigitalNetBase(m_generatingMatrices, m_genValues, m_dimension, m_nRows + inc, m_nCols+inc);
        }

    private:

        DigitalNetBase(std::vector<std::shared_ptr<GeneratingMatrix>> genMatrices,
                       std::vector<std::shared_ptr<GenValue>> genValues,
                       unsigned int dimension,
                       unsigned int nCols,
                       unsigned int nRows):
            DigitalNet(dimension, nRows, nCols, genMatrices),
            m_genValues(genValues)
        {};

        std::vector<std::shared_ptr<GenValue>> m_genValues;
};
}

#endif
