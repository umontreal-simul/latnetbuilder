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


#include "netbuilder/Parser/CommandLine.h"

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
template struct CommandLine<NetConstruction::LMS, EmbeddingType::UNILEVEL>;
template struct CommandLine<NetConstruction::LMS, EmbeddingType::MULTILEVEL>;
template struct CommandLine<NetConstruction::EXPLICIT, EmbeddingType::UNILEVEL>;
template struct CommandLine<NetConstruction::EXPLICIT, EmbeddingType::MULTILEVEL>;
template struct CommandLine<NetConstruction::POLYNOMIAL, EmbeddingType::UNILEVEL>;
template struct CommandLine<NetConstruction::SOBOL, EmbeddingType::UNILEVEL>;
template struct CommandLine<NetConstruction::SOBOL, EmbeddingType::MULTILEVEL>;

}}