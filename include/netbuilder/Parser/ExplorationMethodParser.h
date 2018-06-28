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

#ifndef NETBUILDER__PARSER__EXPLORATION_METHOD_PARSER_H
#define NETBUILDER__PARSER__EXPLORATION_METHOD_PARSER_H

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include "netbuilder/Types.h"
#include "netbuilder/Parser/NetDescriptionParser.h"

#include "netbuilder/Task/Task.h"
#include "netbuilder/Task/CBCSearch.h"
#include "netbuilder/Task/Eval.h"
#include "netbuilder/Task/ExhaustiveSearch.h"
#include "netbuilder/Task/RandomSearch.h"
#include "netbuilder/Task/FullCBCExplorer.h"
#include "netbuilder/Task/MixedCBCExplorer.h"
#include "netbuilder/Task/RandomCBCExplorer.h"
#include "netbuilder/NetConstructionTraits.h"


namespace NetBuilder
{
namespace Parser
{
namespace lbp = LatBuilder::Parser;
/**
 * Exception thrown when trying to parse an invalid size parameter.
 */
class BadExplorationMethod : public lbp::ParserError
{
  public:
    BadExplorationMethod(const std::string &message) : lbp::ParserError("cannot parse exploration method string: " + message)
    {
    }
};

/**
 * Parser for figure parameters.
 */
template<NetConstruction NC, EmbeddingType ET>
struct ExplorationMethodParser
{
    typedef std::unique_ptr<Task::Task> result_type;

    static result_type parse(Parser::CommandLine<NC, ET>& commandLine)
    {
        std::string str = commandLine.s_explorationMethod;
        boost::algorithm::erase_all(str, " ");
        std::vector<std::string> explorationDescriptionStrings;
        boost::split(explorationDescriptionStrings, str, boost::is_any_of(":"));

        std::string name = explorationDescriptionStrings[0];
        if (name == "evaluation"){
            if (explorationDescriptionStrings.size() < 2)
            {
                throw BadExplorationMethod("net description must be specified; see --help");
            }
            auto genValues = NetDescriptionParser<NC,ET>::parse(commandLine, explorationDescriptionStrings[1]);
            auto net = std::make_unique<DigitalNetConstruction<NC>>(commandLine.m_dimension, commandLine.m_sizeParameter, std::move(genValues));
            return std::make_unique<Task::Eval>(std::move(net), std::move(commandLine.m_figure), commandLine.m_verbose);
        }
        else if (name == "exhaustive"){
            return std::make_unique<Task::ExhaustiveSearch<NC, ET>>(commandLine.m_dimension,
                                                        commandLine.m_sizeParameter,
                                                        std::move(commandLine.m_figure),
                                                        commandLine.m_verbose);
        }
        else if (name == "random" || name == "random-CBC" || name == "mixed-CBC"){
            if (explorationDescriptionStrings.size() < 2){
                throw BadExplorationMethod("nb of random tries required; see --help");
            }
            unsigned int r = boost::lexical_cast<unsigned int>(explorationDescriptionStrings[1]);
            if (name == "random")
                return std::make_unique<Task::RandomSearch<NC, ET>>(commandLine.m_dimension,
                                                        commandLine.m_sizeParameter,
                                                        std::move(commandLine.m_figure),
                                                        r,
                                                        commandLine.m_verbose,
                                                        commandLine.m_earlyAbort);
            
            if (name == "random-CBC"){
                return std::make_unique<Task::CBCSearch<NC, ET, Task::RandomCBCExplorer>>(commandLine.m_dimension, 
                                                                commandLine.m_sizeParameter,
                                                                std::move(commandLine.m_figure),
                                                                std::make_unique<Task::RandomCBCExplorer<NC, ET>>(commandLine.m_dimension, commandLine.m_sizeParameter, r),
                                                                commandLine.m_verbose,
                                                                commandLine.m_earlyAbort);
            }

            if (name == "mixed-CBC"){
                if (explorationDescriptionStrings.size() < 3){
                throw BadExplorationMethod("number of fully explored coordinate required; see --help");
                }
                unsigned int nbFullCoordinates = boost::lexical_cast<unsigned int>(explorationDescriptionStrings[2]);

                return std::make_unique<Task::CBCSearch<NC, ET, Task::MixedCBCExplorer>>(commandLine.m_dimension, 
                                                                commandLine.m_sizeParameter,
                                                                std::move(commandLine.m_figure),
                                                                std::make_unique<Task::MixedCBCExplorer<NC, ET>>(commandLine.m_dimension, commandLine.m_sizeParameter, nbFullCoordinates, r), 
                                                                commandLine.m_verbose,
                                                                commandLine.m_earlyAbort);
            }
        }
        else if (name == "full-CBC"){
            return std::make_unique<Task::CBCSearch<NC, ET,  Task::FullCBCExplorer>>(commandLine.m_dimension, 
                                                                commandLine.m_sizeParameter,
                                                                std::move(commandLine.m_figure),
                                                                std::make_unique<Task::FullCBCExplorer<NC, ET>>(commandLine.m_dimension, commandLine.m_sizeParameter),
                                                                commandLine.m_verbose,
                                                                commandLine.m_earlyAbort);
        }
        else{
            throw BadExplorationMethod(name + " is not a valid exploration method; see --help");
        }
        return result_type();
    }


};
}
}

#endif
