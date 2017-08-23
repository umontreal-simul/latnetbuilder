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

   template<Lattice LR>
   static void setFilters(
         LatBuilder::MeritFilterList<LR, LatBuilder::LatType::ORDINARY>& filters,
         const CommandLine<LR, LatBuilder::LatType::ORDINARY>& args,
         const LatBuilder::SizeParam<LR, LatBuilder::LatType::ORDINARY>& size,
         const LatCommon::Weights& weights,
         Real normType
         )
   {
      Parser::MeritFilterList<LR>::parse(
            filters,
            args.filters,
            size,
            weights,
            normType
            );
   }

   template<Lattice LR>
   static void setFilters(
         LatBuilder::MeritFilterList<LR, LatBuilder::LatType::EMBEDDED>& filters,
         const CommandLine<LR, LatBuilder::LatType::EMBEDDED>& args,
         const LatBuilder::SizeParam<LR, LatBuilder::LatType::EMBEDDED>& size,
         const LatCommon::Weights& weights,
         Real normType
         )
   {
      Parser::MeritFilterList<LR>::parse(
            filters,
            args.filters,
            args.multilevelFilters,
            args.combiner,
            size,
            weights,
            normType
            );
   }

   template <LatBuilder::Lattice LR, LatBuilder::LatType LAT>
   class Parse {
   private:
      const CommandLine<LR, LAT>& m_args;
      std::unique_ptr<LatBuilder::Task::Search<LR, LAT>> m_search;

   public:

      Parse(const CommandLine<LR, LAT>& args_): m_args(args_)
      {}


      std::unique_ptr<LatBuilder::Task::Search<LR, LAT>> search()
      {
         Parser::FigureOfMerit<LR>::parse(
               m_args.normType,
               m_args.figure,
               Parser::CombinedWeights::parse(m_args.weights, m_args.weightsPowerScale),
               *this,
               Parser::SizeParam<LR, LAT>::parse(m_args.size),
               boost::lexical_cast<Dimension>(m_args.dimension)
               );
         return std::move(m_search);
      }

      template <class FIGURE>
      void operator()(FIGURE figure, LatBuilder::SizeParam<LR, LAT> size, Dimension dimension)
      {
         m_search = Parser::Search<LR, LAT>::parse(
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

template<>
std::unique_ptr<LatBuilder::Task::Search<Lattice::INTEGRATION, LatBuilder::LatType::ORDINARY>>
CommandLine<Lattice::INTEGRATION, LatBuilder::LatType::ORDINARY>::parse() const
{ return Parse<Lattice::INTEGRATION, LatBuilder::LatType::ORDINARY>(*this).search(); }

template<>
std::unique_ptr<LatBuilder::Task::Search<Lattice::INTEGRATION, LatBuilder::LatType::EMBEDDED>>
CommandLine<Lattice::INTEGRATION, LatBuilder::LatType::EMBEDDED>::parse() const
{ return Parse<Lattice::INTEGRATION, LatBuilder::LatType::EMBEDDED>(*this).search(); }

template<>
std::unique_ptr<LatBuilder::Task::Search<Lattice::POLYNOMIAL, LatBuilder::LatType::ORDINARY>>
CommandLine<Lattice::POLYNOMIAL, LatBuilder::LatType::ORDINARY>::parse() const
{ return Parse<Lattice::POLYNOMIAL, LatBuilder::LatType::ORDINARY>(*this).search(); }

template<>
std::unique_ptr<LatBuilder::Task::Search<Lattice::POLYNOMIAL, LatBuilder::LatType::EMBEDDED>>
CommandLine<Lattice::POLYNOMIAL, LatBuilder::LatType::EMBEDDED>::parse() const
{ return Parse<Lattice::POLYNOMIAL, LatBuilder::LatType::EMBEDDED>(*this).search(); }

/*
template struct CommandLine<LatBuilder::Lattice::INTEGRATION, LatBuilder::LatType::ORDINARY>;
template struct CommandLine<LatBuilder::Lattice::INTEGRATION, LatBuilder::LatType::EMBEDDED>;
template struct CommandLine<LatBuilder::Lattice::POLYNOMIAL, LatBuilder::LatType::ORDINARY>;
template struct CommandLine<LatBuilder::Lattice::POLYNOMIAL, LatBuilder::LatType::EMBEDDED>;
*/

}}
