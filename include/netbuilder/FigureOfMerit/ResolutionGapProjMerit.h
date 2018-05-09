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

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/CBCCoordinateSet.h"
#include "netbuilder/ProgressiveRowReducer.h"

#include "latcommon/Weights.h"
#include "latcommon/Coordinates.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <string>

#include <functional>

namespace NetBuilder { namespace FigureOfMerit {

/** Template class representing a projection-dependent merit defined by the t-value of the projection
 */ 
template <NetEmbed NE>
class ResolutionGapProjMerit
{};

/** Template specialization in the case of simple nets.
 */ 
template <>
class ResolutionGapProjMerit<NetEmbed::SIMPLE>
{
    public:

        /** Construct a projection-dependent merit based on theresolution of projections.
         * @param maxCardinal is the maximum order of the subprojections to take into account
         */  
        ResolutionGapProjMerit(unsigned int maxCardinal,  Combiner combiner = Combiner()):
            m_maxCardinal(maxCardinal)
        {};

        /** Returns the maximum order of the subprojections to take into account.
         */ 
        unsigned int maxCardinal() const { return m_maxCardinal; }

        /** Returns the name of the projection-dependent merit.
         */ 
        std::string name() const { return m_name ;}

        /**  Overload of << operator to print the name of the the projection-dependent merit on the given output stream
         */ 
        friend std::ostream& operator<<(std::ostream& os, const ResolutionGapProjMerit& dt)
        {
            os << "Projection-dependent merit: " << dt.name();
            return os;
        } 


        /** Computes the projection-dependent merit of the net for the given projection.
         * @param net is the digital net for which we want to compute the merit
         * @param projection is the projection to consider
         */ 
        Real operator()(const DigitalNet& net , const LatCommon::Coordinates& projection) 
        {
            unsigned int dimension = projection.size();

            unsigned int numRows = net.numRows();
            unsigned int numCols = net.numColumns();

            m_rowReducer.reset(numCols);

            unsigned int maxResolution = numCols/dimension;
            unsigned int merit = maxResolution; 
            for(unsigned int resolution = 0; resolution < maxResolution; ++resolution)
            {
                GeneratingMatrix block(0,numCols);
                for(const auto& coord : projection)
                {
                    block.vstack(net.pointerToGeneratingMatrix(coord+1)->subMatrix(resolution,1,numCols));
                }
                if (m_rowReducer.reduceNewBlock(std::move(block)))
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

        CBCCoordinateSet projections(unsigned int dimension) const
        {
            return CBCCoordinateSet(dimension, m_maxCardinal);
        }

    private:
        std::string m_name = "resolution (simple nets)"; // name of the projection-dependent merit
        unsigned int m_maxCardinal; // maximum order of subprojections to take into account
        ProgressiveRowReducer m_rowReducer; 
};

/** Template specialization in the case of simple nets.
 */ 
template <>
class ResolutionGapProjMerit<NetEmbed::EMBEDDED>
{
    public:

        /** Construct a projection-dependent merit based on the resolution of projections.
         * @param maxCardinal is the maximum order of the subprojections to take into account
         */  
        ResolutionGapProjMerit(unsigned int maxCardinal, Combiner combiner):
            m_maxCardinal(maxCardinal),
            m_combiner(combiner)
        {};

        /** Returns the maximum order of the subprojections to take into account.
         */ 
        unsigned int maxCardinal() const { return m_maxCardinal; }

        /** Returns the name of the projection-dependent merit.
         */ 
        std::string name() const { return m_name ;}

        /**  Overload of << operator to print the name of the the projection-dependent merit on the given output stream
         */ 
        friend std::ostream& operator<<(std::ostream& os, const ResolutionGapProjMerit& dt)
        {
            os << "Projection-dependent merit: " << dt.name();
            return os;
        } 


        /** Computes the projection-dependent merit of the net for the given projection.
         * @param net is the digital net for which we want to compute the merit
         * @param projection is the projection to consider
         */ 
        Real operator()(const DigitalNet& net , const LatCommon::Coordinates& projection) 
        {
            unsigned int dimension = projection.size();

            unsigned int numRows = net.numRows();
            unsigned int numCols = net.numColumns();

            std::vector<unsigned int> merits(numRows);

            for(unsigned int m = 1; m <= numCols; ++m)
            {
                unsigned int maxResolution = m/dimension;
                merits[m-1] = maxResolution;
                m_rowReducer.reset(m);
                for(unsigned int resolution = 0; resolution < maxResolution; ++resolution)
                {
                    GeneratingMatrix block(0,m);
                    for(const auto& coord : projection)
                    {
                    block.vstack(net.pointerToGeneratingMatrix(coord+1)->subMatrix(resolution,1,m));
                    }
                    if (m_rowReducer.reduceNewBlock(std::move(block)))
                    {
                        --merits[m-1];
                    }
                    else
                    {
                        break;
                    }
                }
            }
            return combine(merits);
        }

        CBCCoordinateSet projections(unsigned int dimension) const
        {
            return CBCCoordinateSet(dimension, m_maxCardinal);
        }

        Real combine(std::vector<unsigned int> merits)
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

