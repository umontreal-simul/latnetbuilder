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

#ifndef NETBUILDER__FIGURE_OF_MERIT_BIT__TVALUE_TRANSFORMED_PROJ_MERIT
#define NETBUILDER__FIGURE_OF_MERIT_BIT__TVALUE_TRANSFORMED_PROJ_MERIT

#include "netbuilder/FigureOfMerit/TValueProjMerit.h"
#include "netbuilder/LevelCombiner.h"

namespace NetBuilder { namespace FigureOfMerit {

using LatticeTester::Coordinates;

namespace{
    /** Function implementing a transformation of the t-value to obtain a transformed t-value based figure of merit.
     *  This function implements :
     *   1) the formula from Corollary 5.3 in \cite rDIC10a
     *  \f[
     *  h(|\mathfrak{u}|, m, t) = 2^{t-m} \sum_{i=0}^{|\mathfrak{u}|-1} \binom{m-t}{i}.
     *  \f]
     *   2) the formula from Exercice 16.11 in \cite rDIC10a
     *  \f[
     *  h(|\mathfrak{u}|, m, t) = \frac{1}{6} + 4^{t} (m-t)^{|\mathfrak{u}|-1}.
     *  \f]
     * @param t t-value of the projection.
     * @param m Number of columns of the digital net matrices.
     * @param s Size of the projection.
     * @param type Identifier of the transformation to use (1 or 2)
     */
    Real h(uInteger t, uInteger m, uInteger s, int type){
        Real res = 0.0;
        if (type == 1){
            Real binom_coeff = 1.0;
            
            for (uInteger i=0; i < std::min(s, m-t+1); i++){
                res += binom_coeff;
                binom_coeff *= (Real) (m-t-i) / (i+1);
            }
            return intPow(0.5, m - t) * res;
        }
        else if (type == 2){
            return 1.0/6 + intPow(4.0, t) * intPow(m-t, s-1);
        }
        else {
            throw std::runtime_error("t-value transformer not implemented.");
        }
    }
}

/** Template class inheriting from NetBuilder::TValueProjMerit to implement a transformed version of the t-value based projection-dependent merit.
 */ 
template <EmbeddingType ET, typename METHOD = GaussMethod>
class TValueTransformedProjMerit: public TValueProjMerit<ET, METHOD>
{
};

/** Template specialization of the projection-dependent merit defined by the transformed t-value of the projection
 *  in the case of unilevel nets. @see NetBuilder::TValueTransformedProjMerit.
 */ 
template <typename METHOD>
class TValueTransformedProjMerit<EmbeddingType::UNILEVEL, METHOD>: public TValueProjMerit<EmbeddingType::UNILEVEL, METHOD>
{
    public:

        /// Type of the merit value
        typedef unsigned int Merit ;

        typedef std::unique_ptr<LevelCombiner::LevelCombiner> pCombiner;

        /** 
         * Constructs a transformed projection-dependent merit based on the t-value of projections.
         * @param maxCardinal Maximum order of the subprojections.
         * @param combiner Not used. For sake of uniformity.
         * @param cost_function Identifier for the figure of merit to be used.
         */  
        TValueTransformedProjMerit(unsigned int maxCardinal, pCombiner combiner=std::make_unique<LevelCombiner::LevelCombiner>(), int cost_function=1): TValueProjMerit<EmbeddingType::UNILEVEL, METHOD>(maxCardinal, std::move(combiner))
        {
            this->cost_function = cost_function;
        }

        virtual Real combine(Merit merit, const DigitalNet& net, const LatticeTester::Coordinates& projection)
        {
            return h(merit, net.numColumns(), projection.size(), cost_function);
        }

        /**
         * Output information about the figure of merit.
         */ 
        virtual std::string format() const
        {
            std::string res;
            res += "Star discrepancy bounding t-value based figure of merit";
            res += "\nEmbedding type: Unilevel";
            return res;
        };

    private:
        int cost_function;
};

/** Template specialization of the projection-dependent merit defined by the t-value of the projection
 *  in the case of multilevel nets. @see NetBuilder::TValueProjMerit.
 */ 
template <typename METHOD>
class TValueTransformedProjMerit<EmbeddingType::MULTILEVEL, METHOD>: public TValueProjMerit<EmbeddingType::MULTILEVEL, METHOD>
{
    public:

        /// Type of the merit value
        typedef std::vector<unsigned int> Merit ;

        typedef std::unique_ptr<LevelCombiner::LevelCombiner> pCombiner;

        /** 
         * Constructs a transformed projection-dependent merit based on the t-value of projections.
         * @param maxCardinal Maximum order of the subprojections.
         * @param combiner Not used. For sake of uniformity.
         * @param cost_function Identifier for the figure of merit to be used.
         */  
        TValueTransformedProjMerit(unsigned int maxCardinal, pCombiner combiner, int cost_function): TValueProjMerit<EmbeddingType::MULTILEVEL, METHOD>(maxCardinal, std::move(combiner))
        {
            this->cost_function = cost_function;
        }

        virtual Real combine(const Merit& merits, const DigitalNet& net, const LatticeTester::Coordinates& projection) {
            RealVector tmp(merits.size());
            for (unsigned int i=0; i<merits.size(); i++){
                tmp[i] = h(merits[i], net.numColumns(), projection.size(), cost_function);
            } 
            return (*(this->m_combiner))(std::move(tmp)) ; 
        }

        /**
         * Output information about the figure of merit.
         */ 
        virtual std::string format() const
        {            
            std::string res;
            res += "Star discrepancy bounding t-value based figure of merit";
            res += "\nEmbedding type: Multilevel";
            res += "\nCombiner: " + this->m_combiner->format();
            return res;
        }; 

    private:
        int cost_function;
};

/**
 * Template specialization of the evaluator for the weighted figure of merit based on the transformed t-value projection-dependent merit 
 * in the case of unilevel nets.
 */ 
template<>
class WeightedFigureOfMerit<TValueTransformedProjMerit<EmbeddingType::UNILEVEL, GaussMethod>>::WeightedFigureOfMeritEvaluator : public ProjectionDependentEvaluator<TValueTransformedProjMerit<EmbeddingType::UNILEVEL, GaussMethod>>
{
    public:

        WeightedFigureOfMeritEvaluator(WeightedFigureOfMerit<TValueTransformedProjMerit<EmbeddingType::UNILEVEL, GaussMethod>>* figure):
            ProjectionDependentEvaluator(figure)
        {}
};

/**
 * Template specialization of the evaluator for the weighted figure of merit based on the transformed t-value projection-dependent merit 
 * in the case of multilevel nets.
 */ 
template<>
class WeightedFigureOfMerit<TValueTransformedProjMerit<EmbeddingType::MULTILEVEL, GaussMethod>>::WeightedFigureOfMeritEvaluator : public ProjectionDependentEvaluator<TValueTransformedProjMerit<EmbeddingType::MULTILEVEL, GaussMethod>>
{
    public:

        WeightedFigureOfMeritEvaluator(WeightedFigureOfMerit<TValueTransformedProjMerit<EmbeddingType::MULTILEVEL, GaussMethod>>* figure):
            ProjectionDependentEvaluator(figure)
        {}
};

/**
 * Template specialization of the evaluator for the weighted figure of merit based on the transformed t-value projection-dependent merit 
 * in the case of unilevel nets.
 */ 
template<>
class WeightedFigureOfMerit<TValueTransformedProjMerit<EmbeddingType::UNILEVEL, SchmidMethod>>::WeightedFigureOfMeritEvaluator : public ProjectionDependentEvaluator<TValueTransformedProjMerit<EmbeddingType::UNILEVEL, SchmidMethod>>
{
    public:

        WeightedFigureOfMeritEvaluator(WeightedFigureOfMerit<TValueTransformedProjMerit<EmbeddingType::UNILEVEL, SchmidMethod>>* figure):
            ProjectionDependentEvaluator(figure)
        {}
};

/**
 * Template specialization of the evaluator for the weighted figure of merit based on the transformed t-value projection-dependent merit 
 * in the case of multilevel nets.
 */ 
template<>
class WeightedFigureOfMerit<TValueTransformedProjMerit<EmbeddingType::MULTILEVEL, SchmidMethod>>::WeightedFigureOfMeritEvaluator : public ProjectionDependentEvaluator<TValueTransformedProjMerit<EmbeddingType::MULTILEVEL, SchmidMethod>>
{
    public:

        WeightedFigureOfMeritEvaluator(WeightedFigureOfMerit<TValueTransformedProjMerit<EmbeddingType::MULTILEVEL, SchmidMethod>>* figure):
            ProjectionDependentEvaluator(figure)
        {}
};



}}

#endif 

