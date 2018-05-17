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
#include "latbuilder/Kernel/RPLR.h"

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/ComputeMaxCardFromWeights.h"
#include "netbuilder/Parser/LevelCombinerParser.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"
#include "netbuilder/FigureOfMerit/TValueProjMerit.h"
#include "netbuilder/FigureOfMerit/ResolutionGapProjMerit.h"
#include "netbuilder/FigureOfMerit/UniformityProperties.h"
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
template<NetConstruction NC, PointSetType PST>
struct FigureParser
{
    typedef std::unique_ptr<FigureOfMerit::FigureOfMerit> result_type;

    static result_type parse(Parser::CommandLine<NC, PST>& commandLine)
    {
        std::vector<std::string> vecStrFigures = commandLine.s_figures;
        std::string &strFigureCombiner = commandLine.s_figureCombiner;

        commandLine.m_combiner = Parser::LevelCombinerParser<NC,PST>::parse(commandLine);

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

    static void parse(std::string str, std::vector<result_type> &vecFigures, std::vector<Real> &vecWeights, CommandLine<NC, PST>& commandLine)
    {
        std::pair<result_type, Real> p;

        // boost::algorithm::erase_all(str, " ");

        std::vector<std::string> figureCharacteristicStrings;
        boost::split(figureCharacteristicStrings, str, boost::is_any_of("/"));

        unsigned int nbParam = (unsigned int) figureCharacteristicStrings.size();
        for (unsigned int i=0; i<nbParam; i++){
            std::vector<std::string> split;
            boost::split(split, figureCharacteristicStrings[i], boost::is_any_of("="));
            if (split.size() == 2){
                figureCharacteristicStrings[i] = split[1];
            }
            // std::cout << figureCharacteristicStrings[i] << std::endl;
        }

        if (figureCharacteristicStrings.size()==0)
        {
            throw BadFigure("figure name must be specified; see --help");
        }

        std::string name = figureCharacteristicStrings[0];

        Real importance = std::numeric_limits<Real>::infinity(); 
        if(figureCharacteristicStrings.size()>=2)
        {
            importance = boost::lexical_cast<Real>(figureCharacteristicStrings[1]);
        }

        if (name == "A-Property"){
            if (NetConstructionTraits<NC>::isSequenceViewable)
            {
                vecFigures.push_back(std::make_unique<FigureOfMerit::AProperty>(importance));
                vecWeights.push_back(1);
                return;
            }
            else
            {
                throw BadFigure("A-Property not compatible with net construction.");
            }
        }
        if (name == "A'-Property"){
            if (NetConstructionTraits<NC>::isSequenceViewable)
            {
                vecFigures.push_back(std::make_unique<FigureOfMerit::APrimeProperty>());
                vecWeights.push_back(1);
                return;
            }
            else
            {
                throw BadFigure("A'-Property not compatible with net construction.");
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

        Real normType = boost::lexical_cast<Real>(figureCharacteristicStrings[1]);

        if (nbParam <=3)
        {
            throw BadFigure("weights must be specified; see --help.");
        }

        vecWeights.push_back(importance);

        std::string weightString = figureCharacteristicStrings[3];
        
        // TODO: comment
        Real weightsPowerScale = 1.0;
        if (nbParam >= 5) {
            Real weightPower = boost::lexical_cast<Real>(figureCharacteristicStrings[4]);

            if (normType < std::numeric_limits<Real>::infinity())
                weightsPowerScale = normType;
         // then scale down according to interpretation of input
            weightsPowerScale /= weightPower;
        }

        std::unique_ptr<LatCommon::Weights> weights = LatBuilder::Parser::Weights::parse(weightString, weightsPowerScale);
        
        // TODO: compute maxCard
        if (name == "t-value"){
            unsigned int maxCard = LatBuilder::WeightsDispatcher::dispatch<ComputeMaxCardFromWeights>(*weights);
            auto projDepMerit = std::make_unique<FigureOfMerit::TValueProjMerit<PST>>(maxCard, commandLine.m_combiner);
            auto weightedFigureOfMerit = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<PST>>>(normType, std::move(weights), std::move(projDepMerit));
            vecFigures.push_back(std::move(weightedFigureOfMerit));
        }
        else if (name == "resolution-gap"){
            unsigned int maxCard = LatBuilder::WeightsDispatcher::dispatch<ComputeMaxCardFromWeights>(*weights);
            auto projDepMerit = std::make_unique<FigureOfMerit::ResolutionGapProjMerit<PST>>(maxCard, commandLine.m_combiner);
            auto weightedFigureOfMerit = std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::ResolutionGapProjMerit<PST>>>(normType, std::move(weights), std::move(projDepMerit));
            vecFigures.push_back(std::move(weightedFigureOfMerit));
        }
        else if (name == "CU:P2"){
            auto kernel = LatBuilder::Kernel::PAlphaPLR(2);
            auto coordUnifFigure = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR, PST>>(std::move(weights), commandLine.m_sizeParam, kernel, commandLine.m_combiner);
            vecFigures.push_back(std::move(coordUnifFigure));
        }
        else if (name == "CU:R"){
            auto kernel = LatBuilder::Kernel::RPLR();
            auto coordUnifFigure = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::RPLR, PST>>(std::move(weights), commandLine.m_sizeParam, kernel, commandLine.m_combiner);
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
