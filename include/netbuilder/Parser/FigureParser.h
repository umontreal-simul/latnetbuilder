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

#ifndef NETBUILDER__PARSER__FIGURE_PARSER_H
#define NETBUILDER__PARSER__FIGURE_PARSER_H

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/Weights.h"
#include "latbuilder/WeightsDispatcher.h"
#include "latbuilder/Kernel/PAlphaPLR.h"
#include "latbuilder/Kernel/RPLR.h"

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/Parser/ComputeMaxCardFromWeights.h"
#include "netbuilder/Parser/LevelCombinerParser.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"
#include "netbuilder/FigureOfMerit/ResolutionGapProjMerit.h"
#include "netbuilder/FigureOfMerit/BitEquidistribution.h"
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
    BadFigure(const std::string &message) : lbp::ParserError("cannot parse figure string: " + message)
    {
    }
};

/**
 * Parser for figure parameters.
 */
template<NetConstruction NC, EmbeddingType ET>
struct FigureParser
{
    typedef std::unique_ptr<FigureOfMerit::FigureOfMerit> result_type;

    static result_type parse(Parser::CommandLine<NC, ET>& commandLine)
    {
        std::vector<std::string> vecStrFigures = commandLine.s_figures;
        std::string &strFigureCombiner = commandLine.s_figureCombiner;

        commandLine.m_combiner = Parser::LevelCombinerParser<NC,ET>::parse(commandLine);

        unsigned int nbFigures = (unsigned int) vecStrFigures.size();
        std::vector<result_type> vecFigures;
        vecFigures.reserve(nbFigures);
        std::vector<Real> vecWeights;
        vecWeights.reserve(nbFigures);

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
            Real normType = -1;
            if (strFigureCombiner=="max")
            {
                normType = std::numeric_limits<Real>::infinity();
            }
            else if (strFigureCombiner=="sum")
            {
                normType = 1;
            }
            else
            {
                throw BadFigure("unrecognized figure combiner; see --help.");
            }
            return std::make_unique<FigureOfMerit::CombinedFigureOfMerit>(normType, std::move(vecFigures), std::move(vecWeights));
        }
    }

    static void parse(std::string str, std::vector<result_type> &vecFigures, std::vector<Real> &vecWeights, CommandLine<NC, ET>& commandLine)
    {
        std::pair<result_type, Real> p;

        // boost::algorithm::erase_all(str, " ");

        std::vector<std::string> figureDescriptionStrings;
        boost::split(figureDescriptionStrings, str, boost::is_any_of("/"));

        unsigned int nbParam = (unsigned int) figureDescriptionStrings.size();
        for (unsigned int i=0; i<nbParam; i++){
            std::vector<std::string> split;
            boost::split(split, figureDescriptionStrings[i], boost::is_any_of("="));
            if (split.size() == 2){
                figureDescriptionStrings[i] = split[1];
            }
            // std::cout << figureDescriptionStrings[i] << std::endl;
        }

        if (figureDescriptionStrings.size()==0)
        {
            throw BadFigure("figure name must be specified; see --help");
        }

        std::string name = figureDescriptionStrings[0];

        Real importance = std::numeric_limits<Real>::infinity(); 
        if(figureDescriptionStrings.size()>=2)
        {
            importance = boost::lexical_cast<Real>(figureDescriptionStrings[1]);
        }

        std::vector<std::string> nameParsing;
        boost::split(nameParsing, name, boost::is_any_of(":"));

        if (nameParsing.size()==2)
        {
            if (nameParsing[0] == "BitEquidistribution")
            {
                unsigned int nbBits = boost::lexical_cast<unsigned int>(nameParsing[1]);
                vecFigures.push_back(std::make_unique<FigureOfMerit::BitEquidistribution<ET>>(nbBits, importance, std::numeric_limits<Real>::infinity(),  commandLine.m_combiner));
                vecWeights.push_back(1);
                return;
            }
        }

        if(nbParam <=1)
        {
            throw BadFigure("importance must be specified; see -help.");
        }

        if(nbParam <=2)
        {
            throw BadFigure("norm-type must be specified; see -help.");
        }

        Real normType = boost::lexical_cast<Real>(figureDescriptionStrings[2]);

        if (nbParam <=3)
        {
            throw BadFigure("weights must be specified; see --help.");
        }

        vecWeights.push_back(importance);

        std::string weightString = figureDescriptionStrings[3];
        
        // interpretation of weights depends on the norm-type and be changed with option --weight-power
        Real weightsPowerScale = 1.0;
        if (commandLine.m_hasWeightPower) {
            if (normType < std::numeric_limits<Real>::infinity())
                weightsPowerScale = normType;
         // then scale down according to interpretation of input
            weightsPowerScale /= commandLine.m_weightPower;
        }

        std::unique_ptr<LatticeTester::Weights> weights = LatBuilder::Parser::Weights::parse(weightString, weightsPowerScale);
        
        if (name == "t-value"){
            unsigned int maxCard = LatBuilder::WeightsDispatcher::dispatch<ComputeMaxCardFromWeights>(*weights);
            auto projDepMerit = std::make_unique<FigureOfMerit::TValueProjMerit<ET>>(maxCard, commandLine.m_combiner);
            auto weightedFigureOfMerit = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<ET>>>(normType, std::move(weights), std::move(projDepMerit));
            vecFigures.push_back(std::move(weightedFigureOfMerit));
        }
        else if (name == "resolution-gap"){
            unsigned int maxCard = LatBuilder::WeightsDispatcher::dispatch<ComputeMaxCardFromWeights>(*weights);
            auto projDepMerit = std::make_unique<FigureOfMerit::ResolutionGapProjMerit<ET>>(maxCard, commandLine.m_combiner);
            auto weightedFigureOfMerit = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::ResolutionGapProjMerit<ET>>>(normType, std::move(weights), std::move(projDepMerit));
            vecFigures.push_back(std::move(weightedFigureOfMerit));
        }
        else if (name == "CU:P2"){
            auto kernel = LatBuilder::Kernel::PAlphaPLR(2);
            auto coordUnifFigure = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR, ET>>(std::move(weights), commandLine.m_sizeParam, kernel, commandLine.m_combiner);
            vecFigures.push_back(std::move(coordUnifFigure));
        }
        else if (name == "CU:R"){
            auto kernel = LatBuilder::Kernel::RPLR();
            auto coordUnifFigure = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::RPLR, ET>>(std::move(weights), commandLine.m_sizeParam, kernel, commandLine.m_combiner);
            vecFigures.push_back(std::move(coordUnifFigure));
        }
        else{
            throw BadFigure("figure name " + name + " does not exist.");
        }
    }


};
}
}

#endif
