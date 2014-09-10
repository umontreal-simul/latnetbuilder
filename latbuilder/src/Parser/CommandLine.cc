// Copyright (c) 2012, 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
// 
// This file is part of Lattice Builder.
// 
// Lattice Builder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Lattice Builder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

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

template struct CommandLine<LatBuilder::LatType::ORDINARY>; // **XX
template struct CommandLine<LatBuilder::LatType::EMBEDDED>; // **XX

}}
