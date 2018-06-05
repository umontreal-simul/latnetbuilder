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

#ifndef NETBUILDER__FIGURE_OF_MERIT_BIT__TVALUE_PROJ_MERIT
#define NETBUILDER__FIGURE_OF_MERIT_BIT__TVALUE_PROJ_MERIT

#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/ProjectionDependentEvaluator.h"

#include "netbuilder/TValueComputation.h"

#include <functional>
#include <stdexcept>

namespace NetBuilder { namespace FigureOfMerit {

using LatticeTester::Coordinates;

/** Template class representing a projection-dependent merit defined by the t-value of the projection
 */ 
template <PointSetType PST>
class TValueProjMerit
{};

/** Template specialization in the case of unilevel nets.
 */ 
template <>
class TValueProjMerit<PointSetType::UNILEVEL>
{
    public:

        /// Type of the merit value
        typedef unsigned int Merit ;

        /// Type of the subprojection combination of merit
        typedef unsigned int SubProjCombination;

        /** 
         * Constructs a projection-dependent merit based on the t-value of projections.
         * @param maxCardinal Maximum order of the subprojections.
         * @param combiner Not used. For sake of uniformity.
         */  
        TValueProjMerit(unsigned int maxCardinal, Combiner combiner=Combiner()):
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
         * Overloads operator << to print the name of the the projection-dependent merit on the given output stream
         */ 
        friend std::ostream& operator<<(std::ostream& os, const TValueProjMerit& dt)
        {
            os << "Projection-dependent merit: " << dt.name();
            return os;
        } 

        /** 
         * Computes the projection-dependent merit of the net \c net for the given projection.
         * @param net Digital net to evaluate.
         * @param projection Projection to use.
         */ 
        Real operator()(const DigitalNet& net , const LatticeTester::Coordinates& projection, SubProjCombination maxMeritsSubProj) const 
        {
            std::vector<GeneratingMatrix> mats;
            for(auto dim : projection)
            {
                mats.push_back(net.generatingMatrix((unsigned int) (dim+1)));
            }
            return SchmidMethod::computeTValue(std::move(mats),maxMeritsSubProj, false);
        }

        Real combine(Merit merit)
        {
            return (Real) merit;
        }
        /** Updates the combination of merit \c subProjCombination using \c merit.
         * @param merit Merit used to update.
         * @param subProjCombination  Combination of merit to update. 
         */ 
        static void update(Merit merit, SubProjCombination& subProjCombination)
        {
            subProjCombination = std::max(merit, subProjCombination);
        }

        /** Initializes the combination of merit \c subProjCombination.
         * @param subProjCombination  Combination of merit to initialized. 
         */ 
        static void setToZero(SubProjCombination& subProjCombination) 
        {
            subProjCombination = 0;
        }

        /**
         * Determines the number of levels from a net.
         */ 
        static unsigned int numLevels(const DigitalNet& net)
        {
            return 1;
        }

        /** 
         * Resize the combination of merit \c subProjCombination to match the number of levels \c numLevels.
         * @param subProjCombination  Combination of merit to resize. 
         */ 
        static void resize(SubProjCombination& subProjCombination, unsigned int numLevels)
        {};

        /**
         * Returns the size of the combination of merit \c subProjCombination.
         */
        static unsigned int size(const SubProjCombination& subProjCombination)
        {
            return 1;
        }


    private:
        std::string m_name = "t-value (unilevel nets)"; // name of the projection-dependent merit
        unsigned int m_maxCardinal; // maximum order of subprojections to take into account 

};

/** Template specialization in the case of multilevel nets.
 */ 
template <>
class TValueProjMerit<PointSetType::MULTILEVEL>
{
    public:

        /// Type of the merit value
        typedef std::vector<unsigned int> Merit ;

        /// Type of the subprojection combination of merit
        typedef std::vector<unsigned int> SubProjCombination;

        /**
         * Construct a projection-dependent merit based on the resolution of projections.
         * @param maxCardinal Maximum order of the subprojections.
         * @param combiner Combiner used to combine multilevel merits.
         */  
        TValueProjMerit(unsigned int maxCardinal, Combiner combiner):
            m_maxCardinal(maxCardinal),
            m_combiner(std::move(combiner))
        {};

        /** 
         * Returns the maximum order of the subprojections to take into account.
         */ 
        unsigned int maxCardinal() const { return m_maxCardinal; }

        /** 
         * Returns the name of the projection-dependent merit.
         */ 
        std::string name() const { return m_name;}

        /**  
         * Overloads operator << to print the name of the the projection-dependent merit on the given output stream.
         */ 
        friend std::ostream& operator<<(std::ostream& os, const TValueProjMerit& dt)
        {
            os << "Projection-dependent merit: " << dt.name();
            return os;
        } 

        /** 
         * Computes the projection-dependent multilevel merits of the net for the given projection.
         * @param net is the digital net for which we want to compute the merit
         * @param projection is the projection to consider
         * @param maxMeritsSubProj is the maximal merit of the subprojections
         */ 
        std::vector<unsigned int> operator()(const DigitalNet& net, const LatticeTester::Coordinates& projection, const std::vector<unsigned int>& maxMeritsSubProj) const 
        {
            std::vector<GeneratingMatrix> mats;
            for(auto dim : projection)
            {
                mats.push_back(net.generatingMatrix((unsigned int) (dim+1)));
            }
            return SchmidMethod::computeTValue(std::move(mats), maxMeritsSubProj, 0);
        }

        /** 
         * Combines the projection-dependent multilevel merits into a single value merit.
         * @param merits Multilevel merits.
         */ 
        Real combine(const Merit& merits) const {
            RealVector tmp(merits.size());
            for (unsigned int i=0; i<merits.size(); i++){
                tmp[i] = (Real) merits[i];
            } 
            return m_combiner(std::move(tmp)) ; 
        }

        /** Updates the combination of merit \c subProjCombination using \c merit.
         * @param merit Merit used to update.
         * @param subProjCombination  Combination of merit to update. 
         */ 
        static void update(Merit merit, SubProjCombination& subProjCombination)
        {
            for(unsigned int i = 0; i < subProjCombination.size(); ++i)
            {
                subProjCombination[i] = std::max(merit[i], subProjCombination[i]);
            }
        }

        /** Initializes the combination of merit \c subProjCombination.
         * @param subProjCombination  Combination of merit to initialized. 
         */ 
        static void setToZero(SubProjCombination& subProjCombination) 
        {
            std::fill(subProjCombination.begin(), subProjCombination.end(), 0);
        }

        /**
         * Determines the number of levels from a net.
         */ 
        static unsigned int numLevels(const DigitalNet& net)
        {
            return net.numColumns();
        }

        /** 
         * Resize the combination of merit \c subProjCombination to match the number of levels \c numLevels.
         * @param subProjCombination  Combination of merit to resize. 
         */ 
        static void resize(SubProjCombination& subProjCombination, unsigned int numLevels)
        {
            subProjCombination.resize(numLevels);
        };

        /**
         * Returns the size of the combination of merit \c subProjCombination.
         */
        static unsigned int size(const SubProjCombination& subProjCombination)
        {
            return (unsigned int) subProjCombination.size();
        }


    private:
        std::string m_name = "t-value (multilevel nets)"; // name of the projection-dependent merit
        unsigned int m_maxCardinal; // maximum order of subprojections to take into account 

        Combiner m_combiner; 
        // function wrapper which combines multilevel merits in a single value merit
};

/**
 * Template specialization of the evaluator for the weighted figure of merit based on the t-value projection-dependent merit 
 * in the case of unilevel nets.
 */ 
template<>
class WeightedFigureOfMerit<TValueProjMerit<PointSetType::UNILEVEL>>::WeightedFigureOfMeritEvaluator : public ProjectionDependentEvaluator<PointSetType::UNILEVEL, TValueProjMerit>
{
    public:

        WeightedFigureOfMeritEvaluator(WeightedFigureOfMerit<TValueProjMerit<PointSetType::UNILEVEL>>* figure):
            ProjectionDependentEvaluator(figure)
        {}
};

/**
 * Template specialization of the evaluator for the weighted figure of merit based on the t-value projection-dependent merit 
 * in the case of multilevel nets.
 */ 
template<>
class WeightedFigureOfMerit<TValueProjMerit<PointSetType::MULTILEVEL>>::WeightedFigureOfMeritEvaluator : public ProjectionDependentEvaluator<PointSetType::MULTILEVEL, TValueProjMerit>
{
    public:

        WeightedFigureOfMeritEvaluator(WeightedFigureOfMerit<TValueProjMerit<PointSetType::MULTILEVEL>>* figure):
            ProjectionDependentEvaluator(figure)
        {}
};



}}

#endif 

