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

#include <limits>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/Weights.h"
#include "latbuilder/Parser/CombinedWeights.h"
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
 * Parser for figures.
 */
template<NetConstruction NC, EmbeddingType ET>
struct FigureParser
{
    typedef std::unique_ptr<FigureOfMerit::CBCFigureOfMerit> result_type;

    static result_type parse(Parser::CommandLine<NC, ET>& commandLine)
    {

        commandLine.m_combiner = Parser::LevelCombinerParser<NC,ET>::parse(commandLine);

        Real weightsPowerScale = 1;
        if (commandLine.m_normType < std::numeric_limits<Real>::infinity())
        {
            weightsPowerScale = commandLine.m_normType / commandLine.m_weightPower;
        }

        std::unique_ptr<LatticeTester::Weights> weights;
        
        if (commandLine.s_weights.size() == 1)
        {
            weights = LatBuilder::Parser::Weights::parse(commandLine.s_weights.front(), weightsPowerScale);
        }
        else
        {
            weights = LatBuilder::Parser::CombinedWeights::parse(commandLine.s_weights, weightsPowerScale);
        }

        if (commandLine.s_figure == "CU:P2")
        {
            if(commandLine.m_normType != 2)
                throw BadFigure("norm must be `2' for the coordinate-uniform implementation");
            auto kernel = LatBuilder::Kernel::PAlphaPLR(2);
            return std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR, ET>>(std::move(weights), kernel, commandLine.m_combiner);
        }
        else if (commandLine.s_figure == "CU:R")
        {
            if(commandLine.m_normType != 2)
                throw BadFigure("norm must be `2' for the coordinate-uniform implementation");
            auto kernel = LatBuilder::Kernel::RPLR();
            return std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::RPLR, ET>>(std::move(weights), kernel, commandLine.m_combiner);
        }
        else if (commandLine.s_figure == "t-value")
        {
            unsigned int maxCard = LatBuilder::WeightsDispatcher::dispatch<ComputeMaxCardFromWeights>(*weights);
            auto projDepMerit = std::make_unique<FigureOfMerit::TValueProjMerit<ET>>(maxCard, commandLine.m_combiner);
            return std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::TValueProjMerit<ET>>>(commandLine.m_normType, std::move(weights), std::move(projDepMerit));
        }
        else if (commandLine.s_figure == "resolution-gap")
        {
            unsigned int maxCard = LatBuilder::WeightsDispatcher::dispatch<ComputeMaxCardFromWeights>(*weights);
            auto projDepMerit = std::make_unique<FigureOfMerit::ResolutionGapProjMerit<ET>>(maxCard, commandLine.m_combiner);
            return std::make_unique<FigureOfMerit::WeightedFigureOfMerit<FigureOfMerit::ResolutionGapProjMerit<ET>>>(commandLine.m_normType, std::move(weights), std::move(projDepMerit));
        }
        else
        {
            throw BadFigure(commandLine.s_figure);
        }
    }


};
}
}

#endif
