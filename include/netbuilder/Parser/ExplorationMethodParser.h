// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

#include "netbuilder/Types.h"
#include "netbuilder/Parser/NetDescriptionParser.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"

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
 * Exception thrown when trying to parse an invalid exploration method.
 */
class BadExplorationMethod : public lbp::ParserError
{
  public:
    BadExplorationMethod(const std::string &message) : lbp::ParserError("cannot parse exploration method string: " + message)
    {
    }
};

/**
 * Parser for exploration methods.
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

        unsigned int r = 0;

        if (name == "evaluation"){
            std::string netDescritionString;
            if (explorationDescriptionStrings.size() < 2 || explorationDescriptionStrings.size() > 3)
            {
                throw BadExplorationMethod("net description is not correctly specified; see --help");
            }
            else if (explorationDescriptionStrings.size() == 2){
                netDescritionString = explorationDescriptionStrings[1];
            }
            else if (explorationDescriptionStrings.size() == 3){
                std::ifstream t(explorationDescriptionStrings[2]);
                std::stringstream buffer;
                buffer << t.rdbuf();
                netDescritionString = buffer.str();
                std::vector<std::string> fileLines;
                boost::split(fileLines, netDescritionString, boost::is_any_of("\n"));
                unsigned int firstLineCoordinate;
                for (unsigned int i=0; i<fileLines.size(); i++){
                    if (fileLines[i][0] == '#'){
                        firstLineCoordinate = i+1;
                    }
                }
                std::vector<std::string> filteredFileLines;
                for (unsigned int i=firstLineCoordinate; i<fileLines.size(); i++){
                    filteredFileLines.push_back(fileLines[i]);
                }
                netDescritionString = boost::algorithm::join(filteredFileLines, "-");
                boost::replace_all(netDescritionString, " ", ",");
            }

            auto genValues = NetDescriptionParser<NC,ET>::parse(commandLine, netDescritionString);
            auto net = std::make_unique<DigitalNet<NC>>(commandLine.m_dimension, commandLine.m_sizeParameter, std::move(genValues));
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
                throw BadExplorationMethod("nb of random samples required; see --help");
            }
            r = boost::lexical_cast<unsigned int>(explorationDescriptionStrings[1]);
        }
        if (name == "random")
            return std::make_unique<Task::RandomSearch<NC, ET>>(commandLine.m_dimension,
                                                    commandLine.m_sizeParameter,
                                                    std::move(commandLine.m_figure),
                                                    r,
                                                    commandLine.m_verbose,
                                                    true);


        std::unique_ptr<FigureOfMerit::CBCFigureOfMerit> figure;
        try{
            auto pfigure = commandLine.m_figure.release();
            figure = std::unique_ptr<FigureOfMerit::CBCFigureOfMerit>(dynamic_cast<FigureOfMerit::CBCFigureOfMerit*> (pfigure));
            if (not(figure))
            {
                throw std::bad_cast();
            }
        } catch (std::bad_cast&)
        {
            throw BadExplorationMethod("the figure of merit cannot be used with CBC explorations.");
        }
            
        if (name == "random-CBC"){
            return std::make_unique<Task::CBCSearch<NC, ET, Task::RandomCBCExplorer>>(commandLine.m_dimension, 
                                                            commandLine.m_sizeParameter,
                                                            std::move(figure),
                                                            std::make_unique<Task::RandomCBCExplorer<NC, ET>>(commandLine.m_dimension, commandLine.m_sizeParameter, r),
                                                            commandLine.m_verbose,
                                                            true);
        }

        if (name == "mixed-CBC"){
            if (explorationDescriptionStrings.size() < 3){
            throw BadExplorationMethod("number of fully explored coordinate required; see --help");
            }
            unsigned int nbFullCoordinates = boost::lexical_cast<unsigned int>(explorationDescriptionStrings[2]);

            return std::make_unique<Task::CBCSearch<NC, ET, Task::MixedCBCExplorer>>(commandLine.m_dimension, 
                                                            commandLine.m_sizeParameter,
                                                            std::move(figure),
                                                            std::make_unique<Task::MixedCBCExplorer<NC, ET>>(commandLine.m_dimension, commandLine.m_sizeParameter, nbFullCoordinates, r), 
                                                            commandLine.m_verbose,
                                                            true);
        }
        else if (name == "full-CBC"){
            return std::make_unique<Task::CBCSearch<NC, ET,  Task::FullCBCExplorer>>(commandLine.m_dimension, 
                                                                commandLine.m_sizeParameter,
                                                                std::move(figure),
                                                                std::make_unique<Task::FullCBCExplorer<NC, ET>>(commandLine.m_dimension, commandLine.m_sizeParameter),
                                                                commandLine.m_verbose,
                                                                true);
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
