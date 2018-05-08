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

#ifndef NETBUILDER__PARSER__FIGURE_PARSER_H
#define NETBUILDER__PARSER__FIGURE_PARSER_H

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/Weights.h"
#include "latbuilder/WeightsDispatcher.h"
#include "latbuilder/Kernel/PAlphaPLR.h"

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/Parser/LevelCombinerParser.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"
#include "netbuilder/FigureOfMerit/CombinedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/CoordUniformFigureOfMerit.h"

namespace NetBuilder
{
namespace Parser
{
namespace lbp = LatBuilder::Parser;
/**
 * Exception thrown when trying to parse an invalid size parameter.
 */
class BadFigure : public lbp::ParserError
{
  public:
    BadFigure(const std::string &message) : lbp::ParserError("cannot parse figure parameter string: " + message)
    {
    }
};

/**
 * Parser for figure parameters.
 */
template<NetConstruction NC, PointSetType PST>
struct FigureParser
{
    typedef std::unique_ptr<FigureOfMerit::FigureOfMerit> result_type;

    static result_type parse(Parser::CommandLine<NC, PST>& commandLine)
    {
        std::vector<std::string> vecStrFigures = commandLine.s_figures;
        std::string &strFigureCombiner = commandLine.s_figureCombiner;
        std::string &strLevelCombiner = commandLine.s_combiner;
        Real normType = boost::lexical_cast<Real>(strFigureCombiner);

        Combiner combiner = Parser::LevelCombinerParser::parse(strLevelCombiner);
        commandLine.m_combiner = combiner;

        int nbFigures = vecStrFigures.size();
        std::vector<result_type> vecFigures;
        vecFigures.reserve(nbFigures);
        std::vector<Real> vecWeights(nbFigures, 0);

        for (const auto &str : vecStrFigures)
        {
            parse(str, vecFigures, vecWeights, commandLine);
        }

        if (nbFigures == 1)
        {
            return std::move(vecFigures[0]);
        }
        else
        {
            return std::make_unique<FigureOfMerit::CombinedFigureOfMerit>(normType, std::move(vecFigures), std::move(vecWeights));
        }
    }

    static void parse(std::string str, std::vector<result_type> &vecFigures, std::vector<Real> &vecWeights, CommandLine<NC, PST>& commandLine)
    {
        std::pair<result_type, Real> p;

        boost::algorithm::erase_all(str, " ");

        std::vector<std::string> figureCharacteristicStrings;
        boost::split(figureCharacteristicStrings, str, boost::is_any_of("|"));
        std::string name = figureCharacteristicStrings[0];
        Real importance = boost::lexical_cast<Real>(figureCharacteristicStrings[1]);

        if (name == "A-Property"){
            if (figureCharacteristicStrings.size() != 2){
                throw BadFigure("Bad number of characteristics for A-Property (see doc)");
            }
            throw BadFigure("not yet implemented");
        }

        Real normType = boost::lexical_cast<Real>(figureCharacteristicStrings[2]);
        std::string weightString = figureCharacteristicStrings[3];
        
        // TODO: comment
        Real weightsPowerScale = 1.0;
        if (figureCharacteristicStrings.size() == 4) {
            Real weightPower = boost::lexical_cast<Real>(figureCharacteristicStrings[4]);

            if (normType < std::numeric_limits<Real>::max())
                weightsPowerScale = normType;
         // then scale down according to interpretation of input
            weightsPowerScale /= weightPower;
        }

        std::unique_ptr<LatCommon::Weights> weights = LatBuilder::Parser::Weights::parse(weightString, weightsPowerScale);
        
        // TODO: compute maxCard
        if (name == "t-value"){
            unsigned int maxCard = LatBuilder::WeightsDispatcher::dispatch<ComputeMaxCardFromWeights>(*weights);
            auto projDepMerit = std::make_unique<FigureOfMerit::TValueProjMerit<PST>>(2, commandLine.m_combiner);
            auto weightedFigureOfMerit = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<PST>>>(normType, std::move(weights), std::move(projDepMerit));
            vecFigures.push_back(std::move(weightedFigureOfMerit));
        }
        else if (name == "resolution-gap"){
            throw BadFigure("not yet implemented");
            // unsigned int maxCard = LatBuilder::WeightsDispatcher::dispatch<ComputeMaxCardFromWeights>(*weights);
            // auto projDepMerit = std::make_unique<FigureOfMerit::ResolutionProjMerit<PST>>(2, commandLine.m_combiner);
            // auto weightedFigureOfMerit = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::ResolutionProjMerit<PST>>>(normType, std::move(weights), std::move(projDepMerit));
            // vecFigures.push_back(std::move(weightedFigureOfMerit));
        }
        else if (name == "CU:P2"){
            auto kernel = LatBuilder::Kernel::PAlphaPLR(2);
            auto coordUnifFigure = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR, PST>>(std::move(weights), commandLine.m_sizeParam, kernel, commandLine.m_combiner);
            vecFigures.push_back(std::move(coordUnifFigure));
        }
        else{
            throw BadFigure("figure name " + name + " does not exist");
        }
        
        vecWeights.push_back(importance);
    }


};
}
}

#endif
