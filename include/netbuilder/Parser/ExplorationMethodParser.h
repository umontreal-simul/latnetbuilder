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

#ifndef NETBUILDER__PARSER__EXPLORATION_METHOD_PARSER_H
#define NETBUILDER__PARSER__EXPLORATION_METHOD_PARSER_H

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include "netbuilder/Types.h"
#include "netbuilder/Task/BaseTask.h"
#include "netbuilder/Task/CBCSearch.h"
#include "netbuilder/Task/Eval.h"
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
    BadExplorationMethod(const std::string &message) : lbp::ParserError("cannot parse exploration method parameter string: " + message)
    {
    }
};

/**
 * Parser for figure parameters.
 */
template<NetConstruction NC, PointSetType PST>
struct ExplorationMethodParser
{
    typedef std::unique_ptr<Task::BaseTask> result_type;

    static result_type parse(Parser::CommandLine<NC, PST>& commandLine)
    {
        std::string str = commandLine.s_explorationMethod;
        boost::algorithm::erase_all(str, " ");
        std::vector<std::string> explorationCharacteristicStrings;
        boost::split(explorationCharacteristicStrings, str, boost::is_any_of(":"));

        std::string name = explorationCharacteristicStrings[0];
        if (name == "explicit"){
            throw BadExplorationMethod("not yet implemented");
        }
        else if (name == "exhaustive"){
            throw BadExplorationMethod("not yet implemented");
        }
        else if (name == "random" || name == "random-CBC" || name == "mixed-CBC"){
            if (explorationCharacteristicStrings.size() < 2){
                throw BadExplorationMethod("nb of random points required; see --help");
            }
            unsigned int r = boost::lexical_cast<unsigned int>(explorationCharacteristicStrings[1]);
            if (name == "random")
                throw BadExplorationMethod("not yet implemented");
            
            if (name == "random-CBC"){
                return std::make_unique<Task::CBCSearch<NC, Task::RandomCBCExplorer>>(commandLine.m_dimension, 
                                                                commandLine.m_sizeParam.log2NumPoints(),
                                                                commandLine.m_sizeParam.log2NumPoints(),
                                                                std::move(commandLine.m_figure),
                                                                std::make_unique<Task::RandomCBCExplorer<NC>>(commandLine.m_dimension, r));
            }

            if (name == "mixed-CBC"){
                if (explorationCharacteristicStrings.size() < 3){
                throw BadExplorationMethod("max exhaustive dimension required; see --help");
                }
                unsigned int maxDim = boost::lexical_cast<unsigned int>(explorationCharacteristicStrings[2]);

                return std::make_unique<Task::CBCSearch<NC, Task::MixedCBCExplorer>>(commandLine.m_dimension, 
                                                                commandLine.m_sizeParam.log2NumPoints(),
                                                                commandLine.m_sizeParam.log2NumPoints(),
                                                                std::move(commandLine.m_figure),
                                                                std::make_unique<Task::MixedCBCExplorer<NC>>(commandLine.m_dimension, maxDim, r));
            }
        }
        else if (name == "full-CBC"){
            return std::make_unique<Task::CBCSearch<NC, Task::FullCBCExplorer>>(commandLine.m_dimension, 
                                                                commandLine.m_sizeParam.log2NumPoints(),
                                                                commandLine.m_sizeParam.log2NumPoints(),
                                                                std::move(commandLine.m_figure),
                                                                std::make_unique<Task::FullCBCExplorer<NC>>(commandLine.m_dimension));

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
