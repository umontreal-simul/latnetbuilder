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

/**
 * \file
 * This file contains the definition of digital nets in base 2.
 */ 

#ifndef NETBUILDER__DIGITAL_NET_H
#define NETBUILDER__DIGITAL_NET_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/GeneratingMatrix.h"
#include "netbuilder/NetConstructionTraits.h"

#include <memory>
#include <sstream>
#include <stdexcept>

namespace NetBuilder {

/** Definition of a digital net in base 2. A digital net is uniquely defined by its set
 * of generating matrices.
*/
class DigitalNet
{
    public:

        /** 
         * Default constructor. 
         * @param dimension Number of coordinates in the net.
         * @param nRows Number of rows of the generating matrices.
         * @param nCols Number of columns of the generating matrices.
        */
        DigitalNet(Dimension dimension=0, unsigned int nRows=0, unsigned int nCols=0):
            m_dimension(dimension),
            m_nRows(nRows),
            m_nCols(nCols)
        {};

        /** 
         * Default destructor.
         */
        virtual ~DigitalNet() = default;

        /** 
         * Returns the number of columns of the generating matrices. 
         */
        unsigned int numColumns() const { return m_nCols; }

        /** 
         * Returns the number of rows of the generating matrices. 
         */
        unsigned int numRows() const { return m_nRows; };

        /** 
         * Returns the number of points of the net 
         */
        uInteger numPoints() const { return intPow(2, m_nCols) ; }

        /** 
         * Returns the number of points of the net 
         */
        uInteger size() const { return intPow(2, m_nCols) ; }

        /** 
         * Returns the dimension (number of coordinates) of the net.
         */
        Dimension dimension() const { return m_dimension ; }

        /** 
         * Returns the generating matrix corresponding to coordinate \c coord.
         * @param coord A coordinate (between 0 and dimension() - 1 ).
         */
        GeneratingMatrix generatingMatrix(Dimension coord) const 
        {
            return m_generatingMatrices[coord]->upperLeftSubMatrix(m_nRows, m_nCols);
        }

        /** 
         * Returns a raw pointer to the generating matrix corresponding to coordinate \c coord.
         * @param coord A coordinate (between 0 and dimension() - 1).
        */
        GeneratingMatrix* pointerToGeneratingMatrix(Dimension coord) const 
        {
            return m_generatingMatrices[coord].get();
        }

        /**
         * Returns a <code>std::string</code> representing the net. The verbosity of the representation
         * depends on \c outputFormat.
         * @param outputFormat Format to use to represent the net.
         */ 
        virtual std::string format(OutputFormat outputFormat) const = 0;

        /** 
         * Returns a bool indicating whether the net can be viewed as a digital sequence.
         */ 
        virtual bool isSequenceViewable() const = 0;

    protected:

        Dimension m_dimension; // dimension of the net
        unsigned int m_nRows; // number of rows in generating matrices
        unsigned int m_nCols; // number of columns in generating matrices
        mutable std::vector<std::shared_ptr<GeneratingMatrix>> m_generatingMatrices; // vector of shared pointers to the generating matrices

        /** 
         * Most general constructor. Designed to be used by derived classes. 
         * @param dimension Dimension of the net.
         * @param nRows Number of rows of the generating matrices.
         * @param nCols Number of columns of the generating matrices.
         * @param genMatrices Vector of shared pointers to the generating matrices.
         */
        DigitalNet(Dimension dimension, unsigned int nRows, unsigned int nCols, std::vector<std::shared_ptr<GeneratingMatrix>> genMatrices):
            m_dimension(dimension),
            m_nRows(nRows),
            m_nCols(nCols),
            m_generatingMatrices(genMatrices)
        {};

};

/** Derived class of DigitalNet designed to implement specific construction methods. The available construction methods
 * are described by the NetConstruction enumeration which is a non-type template parameter of the DigitalNetConstruction 
 * class. @see NetBuilder::NetConstructionTraits. Construction methods are based on two parameters: a size parameter which is 
 * shared by all coordinates and a sequence of generating values, each of them being associated to one coordinate.
 */ 
template <NetConstruction NC>
class DigitalNetConstruction : public DigitalNet
{
    public:

        typedef NetConstructionTraits<NC> ConstructionMethod;

        /// Type of the generating values of the method.
        typedef typename ConstructionMethod::GenValue GenValue;

        /// Type of the size parameter of the method.
        typedef typename ConstructionMethod::SizeParameter SizeParameter;


        /** 
         * Constructor.
         * @param dimension Dimension of the net.
         * @param sizeParameter Size parameter of the net.
         * @param genValues Sequence of generating values to create the net.
         */
        DigitalNetConstruction(
            Dimension dimension,
            SizeParameter sizeParameter, 
            std::vector<GenValue> genValues):
                DigitalNet(dimension, ConstructionMethod::nRows(sizeParameter), ConstructionMethod::nCols(sizeParameter)),
                m_sizeParameter(std::move(sizeParameter))
        {
            m_genValues.reserve(m_dimension);
            for(const auto& genValue : genValues)
            {
                // construct the generating matrix and store them and the generating values
                m_generatingMatrices.push_back(std::shared_ptr<GeneratingMatrix>(ConstructionMethod::createGeneratingMatrix(genValue,m_sizeParameter)));
                m_genValues.push_back(std::shared_ptr<GenValue>(new GenValue(std::move(genValue))));
            }
        }

        /** 
         * Constructor for placeholder nets.
         * @param dimension Dimension of the net.
         * @param sizeParameter Size parameter of the net.
         */
        DigitalNetConstruction(
            Dimension dimension = 0,
            SizeParameter sizeParameter = SizeParameter()):
                DigitalNet(dimension, ConstructionMethod::nRows(sizeParameter), ConstructionMethod::nCols(sizeParameter)),
                m_sizeParameter(std::move(sizeParameter))
        {}

        /**
         * Default destructor
         */
        ~DigitalNetConstruction() = default;

        /** Instantiates a digital net with a dimension increased by one using the generating value \c newGenValue. 
         * Note that the resources (generating matrices, generatins values and computation data) for the lower dimensions are not copied. The net on 
         * which this method is called and the new net share these resources.
         * @param newGenValue  Generating value used to extend the net.
         * @return A <code>std::unique_ptr</code> to the instantiated net.
         */ 
        std::unique_ptr<DigitalNetConstruction<NC>> extendDimension(const GenValue& newGenValue) const 
        {
            std::shared_ptr<GeneratingMatrix> newMat(ConstructionMethod::createGeneratingMatrix(newGenValue,m_sizeParameter));

            // copy the vector of pointers to matrices and add the new matrix
            auto genMats = m_generatingMatrices; 
            genMats.push_back(std::move(newMat));

            // copy the vector of pointers to generating values and add the new generating value
            auto genVals = m_genValues;
            genVals.push_back(std::shared_ptr<GenValue>(new GenValue(newGenValue)));

            // instantiate the new net and return the unique pointer to it
            return std::unique_ptr<DigitalNetConstruction<NC>>(new DigitalNetConstruction<NC>(m_dimension+1, m_sizeParameter, std::move(genVals), std::move(genMats)));
        }


        /**
         * {@inheritDoc}
         */ 
        virtual std::string format(OutputFormat outputFormat) const
        {
            // TODO
            // if (outputFormat == OutputFormat::SSJ){
            //     std::ostringstream out;
            //     out << "2  //Base" << std::endl;
            //     out << numColumns() << "  //Number of columns" << std::endl;
            //     out << numRows() << "  //Number of rows" << std::endl;
            //     out << numPoints() << "  //Number of points" << std::endl;
            //     out << dimension() << "  //dimension of points" << std::endl;
            //     out << std::endl;
            //     for(uInteger j=0; j<dimension(); j++){
            //         out << "//dim = " << j+1 << std::endl;
            //         std::vector<uInteger> cols = generatingMatrix(j).getColsReverse();
            //         for(uInteger c=0; c<numColumns(); c++){
            //             uInteger x = cols[c];
            //             uInteger mult = intPow(2,30);
            //             uInteger res = 0;
            //             while(x > 0){
            //                 res += (x%2) * mult ;
            //                 x /= 2;
            //                 mult /= 2;
            //             }
            //             out << res << std::endl;
            //         }
            //         out << std::endl;
            //     }
            //     return out.str();
            // }
            if (outputFormat == OutputFormat::SSJ){
                std::ostringstream out;
                out << "2  //Base" << std::endl;
                out << numColumns() << "  //Number of columns" << std::endl;
                out << numRows() << "  //Number of rows" << std::endl;
                out << numPoints() << "  //Number of points" << std::endl;
                out << dimension() << "  //dimension of points" << std::endl;
                out << std::endl;
                for(Dimension dim = 0; dim < m_dimension; ++dim)
                {
                    out << "//dim = " << dim << std::endl;
                    out << generatingMatrix(dim) << std::endl;
                }
                return out.str();
            }

            std::string res = ConstructionMethod::format(m_genValues,m_sizeParameter,outputFormat);
            if (outputFormat==OutputFormat::GUI)
            {
                std::ostringstream stream;
                for(Dimension dim = 1; dim <= m_dimension; ++dim)
                {
                    stream << "//dim = " << dim << std::endl;
                    stream << generatingMatrix(dim) << std::endl;
                }
                res+="\n\n";
                res+=stream.str();
            }
            return res;
        }

        /**
         * {@inheritDoc}
         */ 
        virtual bool isSequenceViewable() const 
        {
            return ConstructionMethod::isSequenceViewable;
        }

        SizeParameter sizeParameter() const { return m_sizeParameter ; }
    
    private:

        SizeParameter m_sizeParameter; // size parameter of the net
        mutable std::vector<std::shared_ptr<GenValue>> m_genValues; // vector of shared pointers to the generating values of the net TODO change mutable

        /** Most general constructor.
         * @param dimension Dimension of the net.
         * @param sizeParameter Size parameter of the net.
         * @param genValues Vector of shared pointers to the generating values of each coordinate.
         * @param genMatrices Vector of shared pointers to the generating matrices of each coordinate.
        */ 
        DigitalNetConstruction(
            Dimension dimension,
            SizeParameter sizeParameter,
            std::vector<std::shared_ptr<GenValue>> genValues,
            std::vector<std::shared_ptr<GeneratingMatrix>> genMatrices):
                DigitalNet(dimension, ConstructionMethod::nRows(sizeParameter), ConstructionMethod::nCols(sizeParameter), std::move(genMatrices)),
                m_sizeParameter(std::move(sizeParameter)),
                m_genValues(std::move(genValues))
        {};
};
}

#endif
