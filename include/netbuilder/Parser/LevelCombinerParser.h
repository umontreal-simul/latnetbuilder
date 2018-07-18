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

#ifndef NETBUILDER__PARSER__LEVEL_COMBINER_PARSER_H
#define NETBUILDER__PARSER__LEVEL_COMBINER_PARSER_H

#include "latbuilder/Parser/Common.h"
#include "netbuilder/Types.h"
#include "netbuilder/LevelCombiner.h"
#include "netbuilder/NetConstructionTraits.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace NetBuilder { namespace Parser {
namespace lbp = LatBuilder::Parser;

// using namespace NetBuilder::LevelCombiner;

typedef LevelCombiner::LevelCombiner Combiner;

/**
 * Exception thrown when trying to parse an invalid combiner.
 */
class BadLevelCombiner : public lbp::ParserError {
public:
   BadLevelCombiner(const std::string& message):
      lbp::ParserError("cannot parse combiner string: " + message)
   {}
};

/**
 * Parser for combiners.
 */
template <NetConstruction NC, NetBuilder::EmbeddingType ET>
struct LevelCombinerParser {};

template <NetConstruction NC>
struct LevelCombinerParser<NC, NetBuilder::EmbeddingType::UNILEVEL> {
//    typedef Combiner result_type;

   static std::unique_ptr<Combiner> parse(const CommandLine<NC,NetBuilder::EmbeddingType::UNILEVEL>& commandLine)
   {
       return std::make_unique<Combiner>();
   }
};

template <NetConstruction NC>
struct LevelCombinerParser<NC, NetBuilder::EmbeddingType::MULTILEVEL> {
//    typedef Combiner result_type;

   static std::unique_ptr<Combiner> parse(const CommandLine<NC,NetBuilder::EmbeddingType::MULTILEVEL>& commandLine)
   {
       std::string str = commandLine.s_combiner;
       if (str=="sum")
       {
           return std::make_unique<LevelCombiner::SumCombiner>();
       }
       else if (str=="max")
       {
           return std::make_unique<LevelCombiner::MaxCombiner>();
       }
       else
       {
            std::vector<std::string> combinerStrings;
            boost::split(combinerStrings, str, boost::is_any_of(":"));
            if (combinerStrings.size()!=2 || combinerStrings[0] != "level")
            {
                throw BadLevelCombiner(str);
            }
            else
            {
                unsigned int level;
                if (combinerStrings[1] == "max")
                {
                    level = NetConstructionTraits<NC>::nCols(commandLine.m_sizeParameter);
                }
                else
                {
                    level = boost::lexical_cast<unsigned int>(combinerStrings[1]);
                    if (level > NetConstructionTraits<NC>::nCols(commandLine.m_sizeParameter) || level == 0)
                    {
                       throw BadLevelCombiner("incompatible combiner level and size.");
                    }  
                }
                return std::make_unique<LevelCombiner::LevelSelectorCombiner>(level);
            }
       }
       throw BadLevelCombiner(str);
       return std::make_unique<Combiner>();
   }
};

}}

#endif
