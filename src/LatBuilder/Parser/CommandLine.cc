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

   template<LatticeType LR>
   static void setFilters(
         LatBuilder::MeritFilterList<LR, LatBuilder::PointSetType::UNILEVEL>& filters,
         const CommandLine<LR, LatBuilder::PointSetType::UNILEVEL>& args,
         const LatBuilder::SizeParam<LR, LatBuilder::PointSetType::UNILEVEL>& size,
         const LatticeTester::Weights& weights,
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

   template<LatticeType LR>
   static void setFilters(
         LatBuilder::MeritFilterList<LR, LatBuilder::PointSetType::MULTILEVEL>& filters,
         const CommandLine<LR, LatBuilder::PointSetType::MULTILEVEL>& args,
         const LatBuilder::SizeParam<LR, LatBuilder::PointSetType::MULTILEVEL>& size,
         const LatticeTester::Weights& weights,
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

   template <LatBuilder::LatticeType LR, LatBuilder::PointSetType PST>
   class Parse {
   private:
      const CommandLine<LR, PST>& m_args;
      std::unique_ptr<LatBuilder::Task::Search<LR, PST>> m_search;

   public:

      Parse(const CommandLine<LR, PST>& args_): m_args(args_)
      {}


      std::unique_ptr<LatBuilder::Task::Search<LR, PST>> search()
      {
         Parser::FigureOfMerit<LR>::parse(
               m_args.normType,
               m_args.figure,
               Parser::CombinedWeights::parse(m_args.weights, m_args.weightsPowerScale),
               *this,
               Parser::SizeParam<LR, PST>::parse(m_args.size),
               boost::lexical_cast<Dimension>(m_args.dimension)
               );
         return std::move(m_search);
      }

      template <class FIGURE>
      void operator()(FIGURE figure, LatBuilder::SizeParam<LR, PST> size, Dimension dimension)
      {
         m_search = Parser::Search<LR, PST>::parse(
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
std::unique_ptr<LatBuilder::Task::Search<LatticeType::ORDINARY, LatBuilder::PointSetType::UNILEVEL>>
CommandLine<LatticeType::ORDINARY, LatBuilder::PointSetType::UNILEVEL>::parse() const
{ return Parse<LatticeType::ORDINARY, LatBuilder::PointSetType::UNILEVEL>(*this).search(); }

template<>
std::unique_ptr<LatBuilder::Task::Search<LatticeType::ORDINARY, LatBuilder::PointSetType::MULTILEVEL>>
CommandLine<LatticeType::ORDINARY, LatBuilder::PointSetType::MULTILEVEL>::parse() const
{ return Parse<LatticeType::ORDINARY, LatBuilder::PointSetType::MULTILEVEL>(*this).search(); }

template<>
std::unique_ptr<LatBuilder::Task::Search<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::UNILEVEL>>
CommandLine<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::UNILEVEL>::parse() const
{ return Parse<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::UNILEVEL>(*this).search(); }

template<>
std::unique_ptr<LatBuilder::Task::Search<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::MULTILEVEL>>
CommandLine<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::MULTILEVEL>::parse() const
{ return Parse<LatticeType::POLYNOMIAL, LatBuilder::PointSetType::MULTILEVEL>(*this).search(); }

/*
template struct CommandLine<LatBuilder::LatticeType::ORDINARY, LatBuilder::PointSetType::UNILEVEL>;
template struct CommandLine<LatBuilder::LatticeType::ORDINARY, LatBuilder::PointSetType::MULTILEVEL>;
template struct CommandLine<LatBuilder::LatticeType::POLYNOMIAL, LatBuilder::PointSetType::UNILEVEL>;
template struct CommandLine<LatBuilder::LatticeType::POLYNOMIAL, LatBuilder::PointSetType::MULTILEVEL>;
*/

}}
