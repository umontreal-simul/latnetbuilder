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

#include "latbuilder/Parser/CommandLine.h"
#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/SizeParam.h"
#include "latbuilder/Parser/CombinedWeights.h"
#include "latbuilder/Parser/FigureOfMerit.h"
#include "latbuilder/Parser/MeritFilterList.h"
#include "latbuilder/Parser/Search.h"

#include <boost/lexical_cast.hpp>

namespace LatBuilder { namespace Parser {

namespace {

   static void setFilters(
         LatBuilder::MeritFilterList<LatBuilder::LatType::ORDINARY>& filters,
         const CommandLine<LatBuilder::LatType::ORDINARY>& args,
         const LatBuilder::SizeParam<LatBuilder::LatType::ORDINARY>& size,
         const LatCommon::Weights& weights,
         Real normType
         )
   {
      Parser::MeritFilterList::parse(
            filters,
            args.filters,
            size,
            weights,
            normType
            );
   }

   static void setFilters(
         LatBuilder::MeritFilterList<LatBuilder::LatType::EMBEDDED>& filters,
         const CommandLine<LatBuilder::LatType::EMBEDDED>& args,
         const LatBuilder::SizeParam<LatBuilder::LatType::EMBEDDED>& size,
         const LatCommon::Weights& weights,
         Real normType
         )
   {
      Parser::MeritFilterList::parse(
            filters,
            args.filters,
            args.multilevelFilters,
            args.combiner,
            size,
            weights,
            normType
            );
   }

   template <LatBuilder::LatType LAT>
   class Parse {
   private:
      const CommandLine<LAT>& m_args;
      std::unique_ptr<LatBuilder::Task::Search<LAT>> m_search;

   public:

      Parse(const CommandLine<LAT>& args_): m_args(args_)
      {}


      std::unique_ptr<LatBuilder::Task::Search<LAT>> search()
      {
         Parser::FigureOfMerit::parse(
               m_args.normType,
               m_args.figure,
               Parser::CombinedWeights::parse(m_args.weights, m_args.weightsPowerScale),
               *this,
               Parser::SizeParam::parse<LAT>(m_args.size),
               boost::lexical_cast<Dimension>(m_args.dimension)
               );
         return std::move(m_search);
      }

      template <class FIGURE>
      void operator()(FIGURE figure, LatBuilder::SizeParam<LAT> size, Dimension dimension)
      {
         m_search = Parser::Search<LAT>::parse(
               m_args.construction,
               size,
               dimension,
               std::move(figure));
         setFilters(
               m_search->filters(),
               m_args,
               size,
               m_search->figureOfMerit().weights(),
               m_search->figureOfMerit().normType());
      }
   };

}

std::unique_ptr<LatBuilder::Task::Search<LatBuilder::LatType::ORDINARY>>
CommandLine<LatBuilder::LatType::ORDINARY>::parse() const
{ return Parse<LatBuilder::LatType::ORDINARY>(*this).search(); }

std::unique_ptr<LatBuilder::Task::Search<LatBuilder::LatType::EMBEDDED>>
CommandLine<LatBuilder::LatType::EMBEDDED>::parse() const
{ return Parse<LatBuilder::LatType::EMBEDDED>(*this).search(); }

template struct CommandLine<LatBuilder::LatType::ORDINARY>;
template struct CommandLine<LatBuilder::LatType::EMBEDDED>;

}}
