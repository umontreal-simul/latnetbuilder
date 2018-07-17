// This file is part of Nettice Builder.
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

#ifndef NETBUILDER__PARSER__COMMAND_LINE_H
#define NETBUILDER__PARSER__COMMAND_LINE_H

#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"
#include "netbuilder/Task/Task.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "netbuilder/LevelCombiner.h"

#include "latbuilder/SizeParam.h"

namespace NetBuilder { namespace Parser {

/**
 * Collection of arguments required to construct a Search instance.
 */
// template <NetBuilder::NetConstruction , NetBuilder::EmbeddingType>
// struct CommandLine;

/**
 * Specialization of CommandLine for ordinary nets.
 */
template <NetConstruction NC, EmbeddingType ET>
struct CommandLine {
   std::string s_explorationMethod;
   std::string s_size;
   std::string s_dimension;
   std::string s_figure;
   std::vector<std::string> s_weights;
   std::string s_figureCombiner;
   std::string s_combiner;
   std::string s_verbose;
   
   bool m_earlyAbort;
   Real m_normType;
   Real m_weightPower;
   typename NetConstructionTraits<NC>::SizeParameter m_sizeParameter;
   std::unique_ptr<LevelCombiner::LevelCombiner> m_combiner;
   Dimension m_dimension;
   std::unique_ptr<FigureOfMerit::FigureOfMerit> m_figure;
   int m_verbose;
   unsigned int m_interlacingFactor;

   std::unique_ptr<Task::Task> parse();
};

}}

#include "netbuilder/Parser/SizeParameterParser.h"
#include "netbuilder/Parser/FigureParser.h"
#include "netbuilder/Parser/ExplorationMethodParser.h"

namespace NetBuilder { namespace Parser {
template <NetConstruction NC, EmbeddingType ET>
std::unique_ptr<NetBuilder::Task::Task>
CommandLine<NC, ET>::parse()
{
      namespace lbp = LatBuilder::Parser;
      
      SizeParameterParser<NC,ET>::parse(*this);
      m_dimension = boost::lexical_cast<Dimension>(s_dimension) * m_interlacingFactor;
      if (m_interlacingFactor > 1){
            std::cout << "Warning: interlacing factor is > 1." << std::endl;
            std::cout << "    Dimension: " <<  boost::lexical_cast<Dimension>(s_dimension) << std::endl;
            std::cout << "    Interlacing factor: " << m_interlacingFactor << std::endl;
            std::cout << "    Number of components: " << m_dimension << std::endl;
      }
      m_verbose = boost::lexical_cast<int>(s_verbose);
      m_figure = FigureParser<NC, ET>::parse(*this); // m_combiner initialized and moved to m_figure as a side effect 
      return ExplorationMethodParser<NC, ET>::parse(*this); // as a side effect, m_figure has been moved to task
}


}}

#endif
