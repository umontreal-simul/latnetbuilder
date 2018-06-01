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

#ifndef NETBUILDER__RESOLUTION_GAP_PROJ_MERIT
#define NETBUILDER__RESOLUTION_GAP_PROJ_MERIT

#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/CBCCoordinateSet.h"
#include "netbuilder/ProgressiveRowReducer.h"

namespace NetBuilder { namespace FigureOfMerit {

/** 
 * Template class representing a projection-dependent merit defined by the resolution-gap of the projection.
 * The resolution gap is defined as follows. Let \f l \f be the maximal integer such that the net is \f (l, \dots, l) \f equidistributed.
 * We have a natural upper-bound on \f l \f given by \f \frac{m}{s} \f where \f m \f is the number of columns of the generating matrices and
 * \f s \f is the order of the projection. \f l \f is called the resolution of the net and the resolution-gap is defined by the difference \f \frac{m}{s} - l \f.
 */ 
template <PointSetType PST>
class ResolutionGapProjMerit
{};

/** Template specialization in the case of simple nets.
 */ 
template <>
class ResolutionGapProjMerit<PointSetType::UNILEVEL>
{
    public:

        /** 
         * Construct a projection-dependent merit based on the resolution of projections.
         * @param maxCardinal Maximum order of the subprojections.
         * @param combiner Not used. For sake of uniformity.
         */  
        ResolutionGapProjMerit(unsigned int maxCardinal,  Combiner combiner = Combiner()):
            m_maxCardinal(maxCardinal)
        {};

        /** 
         * Returns the maximum order of the subprojections to take into account.
         */ 
        unsigned int maxCardinal() const { return m_maxCardinal; }

        /** 
         * Returns the name of the projection-dependent merit.
         */ 
        std::string name() const { return m_name ;}

        /**  
         * Overloads of << operator to print the name of the the projection-dependent merit on the given output stream
         */ 
        friend std::ostream& operator<<(std::ostream& os, const ResolutionGapProjMerit& dt)
        {
            os << "Projection-dependent merit: " << dt.name();
            return os;
        } 


        /** 
         * Computes the projection-dependent merit of the net \c net for the given projection.
         * @param net Digital to evaluate.
         * @param projection Projection to use.
         */ 
        Real operator()(const DigitalNet& net , const LatticeTester::Coordinates& projection) 
        {
            unsigned int dimension = (unsigned int) projection.size();
            unsigned int numCols = net.numColumns();

            m_rowReducer.reset(numCols);

            unsigned int maxResolution = numCols/dimension;
            unsigned int merit = maxResolution; 
            for(unsigned int resolution = 0; resolution < maxResolution; ++resolution)
            {
                for(auto coord : projection)
                {
                    m_rowReducer.addRow(net.pointerToGeneratingMatrix((unsigned int) (coord+1))->subMatrix(resolution,1,numCols));
                }
                if(m_rowReducer.computeRank() == m_rowReducer.numRows())
                {
                    --merit;
                }
                else
                {
                    break;
                }
            }
            return  merit;
        }

        /** 
         * Returns the projections to include in the figure of merit partial computation for dimension \c dimension.
         * @param dimension Dimension of the partial computation.
         */ 
        CBCCoordinateSet projections(unsigned int dimension) const
        {
            return CBCCoordinateSet(dimension, m_maxCardinal);
        }

    private:
        std::string m_name = "resolution (simple nets)"; // name of the projection-dependent merit
        unsigned int m_maxCardinal; // maximum order of subprojections to take into account
        ProgressiveRowReducer m_rowReducer; // use to compute the rank of matrices
};

/** Template specialization in the case of simple nets.
 */ 
template <>
class ResolutionGapProjMerit<PointSetType::MULTILEVEL>
{
    public:

        /** 
         * Construct a projection-dependent merit based on the resolution of projections.
         * @param maxCardinal Maximum order of the subprojections.
         * @param combiner Combiner used to combine multilevel merits.
         */   
        ResolutionGapProjMerit(unsigned int maxCardinal, Combiner combiner):
            m_maxCardinal(maxCardinal),
            m_combiner(combiner)
        {};

        /** 
         * Returns the maximum order of the subprojections to take into account.
         */ 
        unsigned int maxCardinal() const { return m_maxCardinal; }

        /** 
         * Returns the name of the projection-dependent merit.
         */ 
        std::string name() const { return m_name ;}

        /**  
         * Overloads of << operator to print the name of the the projection-dependent merit on the given output stream
         */ 
        friend std::ostream& operator<<(std::ostream& os, const ResolutionGapProjMerit& dt)
        {
            os << "Projection-dependent merit: " << dt.name();
            return os;
        } 


        /** 
         * Computes the projection-dependent merit of the net \c net for the given projection.
         * @param net Digital to evaluate.
         * @param projection Projection to use.
         */ 
        Real operator()(const DigitalNet& net , const LatticeTester::Coordinates& projection) 
        {
            unsigned int dimension = (unsigned int) projection.size();

            unsigned int numRows = net.numRows();
            unsigned int numCols = net.numColumns();

            m_rowReducer.reset(numCols);

            std::vector<unsigned int> merits(numRows);

            for(unsigned int m = 1; m <= numCols; ++m)
            {
                merits[m-1] = m/dimension;
            }

            unsigned int maxResolution = numCols/dimension;

            for(unsigned int resolution = 0; resolution < maxResolution; ++resolution)
            {
                for(auto coord : projection)
                {
                    m_rowReducer.addRow(net.pointerToGeneratingMatrix((unsigned int) (coord+1))->subMatrix(resolution,1,numCols));
                }
                std::vector<unsigned int> ranks = m_rowReducer.computeRanks(0,numCols);
                for(unsigned int m = 1; m <= numCols; ++m)
                {
                    if (ranks[m-1] == m_rowReducer.numRows())
                    {
                        --merits[m-1];
                    }
                }
                if (ranks[numCols-1] <  m_rowReducer.numRows())
                {
                    break;
                }
            }
            return combine(merits);
        }


        /** 
         * Returns the projections to include in the figure of merit partial computation for dimension \c dimension.
         * @param dimension Dimension of the partial computation.
         */ 
        CBCCoordinateSet projections(unsigned int dimension) const
        {
            return CBCCoordinateSet(dimension, m_maxCardinal);
        }

        /** 
         * Combines the vector of multilevel merits into a single value merit.
         * @param merits Multilevel merits to combine.
         */ 
        Real combine(const std::vector<unsigned int>& merits)
        {
            RealVector tmp(merits.size());
            for(unsigned int i = 0; i < merits.size(); ++i)
            {
                tmp[i] = merits[i];
            }
            return m_combiner(std::move(tmp));
        }

    private:
        std::string m_name = "resolution (embedded nets)"; // name of the projection-dependent merit
        unsigned int m_maxCardinal; // maximum order of subprojections to take into account 
        Combiner m_combiner; 
        ProgressiveRowReducer m_rowReducer;
};

}}

#endif 

