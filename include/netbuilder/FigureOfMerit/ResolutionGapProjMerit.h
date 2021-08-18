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

#ifndef NETBUILDER__FIGURE_OF_MERIT_BIT__RESOLUTION_GAP_PROJ_MERIT
#define NETBUILDER__FIGURE_OF_MERIT_BIT__RESOLUTION_GAP_PROJ_MERIT

#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/Helpers/CBCCoordinateSet.h"
#include "netbuilder/Helpers/RankComputer.h"
#include "netbuilder/FigureOfMerit/LevelCombiner.h"

namespace NetBuilder { namespace FigureOfMerit {

/** 
 * Template class representing a projection-dependent merit defined by the resolution-gap of the projection.
 * The resolution gap is defined as follows. Let \f$ l \f$ be the maximal integer such that the net is \f$ (l, \dots, l) \f$ equidistributed.
 * We have a natural upper-bound on \f$ l \f$ given by \f$ \frac{m}{s} \f$ where \f$ m \f$ is the number of columns of the generating matrices and
 * \f$ s \f$ is the order of the projection. \f$ l \f$ is called the resolution of the net and the resolution-gap is defined by the difference \f$ \frac{m}{s} - l \f$.
 * @tparam ET Embedding type : UNILEVEL or MULTILEVEL.
 */ 
template <EmbeddingType ET>
class ResolutionGapProjMerit
{};

/** Template specialization of the projection-dependent merit defined by the resolution-gap of the projection
 *  in the case of unilevel nets. @see NetBuilder::ResolutionGapProjMerit
 */ 
template <>
class ResolutionGapProjMerit<EmbeddingType::UNILEVEL>
{
    public:

        typedef std::unique_ptr<LevelCombiner::LevelCombiner> pCombiner;

        /** 
         * Constructs a projection-dependent merit based on the resolution-gap of projections.
         * @param maxCardinal Maximum order of the subprojections.
         * @param combiner Not used. For sake of uniformity.
         */  
        ResolutionGapProjMerit(unsigned int maxCardinal,  pCombiner combiner = std::make_unique<LevelCombiner::LevelCombiner>()):
            m_maxCardinal(maxCardinal)
        {};

        /** 
         * Returns the maximum order of the subprojections to take into account.
         */ 
        unsigned int maxCardinal() const { return m_maxCardinal; }

        /**
         * Output information about the figure of merit.
         */ 
        std::string format() const
        {            
            std::string res;
            res += "resolution (unilevel nets)";
            res += "\nEmbedding type: Unilevel";
            return res;
        }; 

        /** 
         * Computes the projection-dependent merit of the net \c net for the given projection.
         * @param net Digital to evaluate.
         * @param projection Projection to use.
         */ 
        Real operator()(const AbstractDigitalNet& net , const LatticeTester::Coordinates& projection) 
        {
            Dimension dimension = projection.size();
            unsigned int numCols = net.numColumns();

            m_rankComputer.reset(numCols);

            unsigned int maxResolution = numCols/dimension;
            unsigned int merit = maxResolution; 
            for(unsigned int resolution = 0; resolution < maxResolution; ++resolution)
            {
                for(auto coord : projection)
                {
                    m_rankComputer.addRow(net.generatingMatrix(coord).subMatrix(resolution, 0, 1,numCols));
                }
                if(m_rankComputer.computeRank() == m_rankComputer.numRows())
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
        CBCCoordinateSet projections(Dimension dimension) const
        {
            return CBCCoordinateSet(dimension, m_maxCardinal);
        }

    private:
        unsigned int m_maxCardinal; // maximum order of subprojections to take into account
        RankComputer m_rankComputer; // use to compute the rank of matrices
};

/** Template specialization of the projection-dependent merit defined by the resolution-gap of the projection
 *  in the case of multilevel nets. @see NetBuilder::ResolutionGapProjMerit
 */ 
template <>
class ResolutionGapProjMerit<EmbeddingType::MULTILEVEL>
{
    public:

        typedef std::unique_ptr<LevelCombiner::LevelCombiner> pCombiner;

        /** 
         * Constructs a projection-dependent merit based on the resolution-gap of projections.
         * @param maxCardinal Maximum order of the subprojections.
         * @param combiner LevelCombiner used to combine multilevel merits.
         */   
        ResolutionGapProjMerit(unsigned int maxCardinal, pCombiner combiner = std::make_unique<LevelCombiner::LevelCombiner>()):
            m_maxCardinal(maxCardinal),
            m_combiner(std::move(combiner))
        {};

        /** 
         * Returns the maximum order of the subprojections to take into account.
         */ 
        unsigned int maxCardinal() const { return m_maxCardinal; }

        /**
         * Output information about the figure of merit.
         */ 
        std::string format() const
        {            
            std::string res;
            res += "resolution (multilevel nets)";
            res += "\nEmbedding type: Multilevel";
            res += "\nCombiner: " + this->m_combiner->format();
            return res;
        }; 


        /** 
         * Computes the projection-dependent merit of the net \c net for the given projection.
         * @param net Digital to evaluate.
         * @param projection Projection to use.
         */ 
        Real operator()(const AbstractDigitalNet& net , const LatticeTester::Coordinates& projection) 
        {
            Dimension dimension = projection.size();

            unsigned int numRows = net.numRows();
            unsigned int numCols = net.numColumns();

            m_rankComputer.reset(numCols);

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
                    m_rankComputer.addRow(net.generatingMatrix(coord).subMatrix(resolution, 0,  1, numCols));
                }
                std::vector<unsigned int> ranks = m_rankComputer.computeRanks(0,numCols);
                for(unsigned int m = 1; m <= numCols; ++m)
                {
                    if (ranks[m-1] == m_rankComputer.numRows())
                    {
                        --merits[m-1];
                    }
                }
                if (ranks[numCols-1] <  m_rankComputer.numRows())
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
        CBCCoordinateSet projections(Dimension dimension) const
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
            return (*m_combiner)(std::move(tmp));
        }

    private:
        unsigned int m_maxCardinal; // maximum order of subprojections to take into account 
        pCombiner m_combiner; 
        RankComputer m_rankComputer;
};

}}

#endif 

