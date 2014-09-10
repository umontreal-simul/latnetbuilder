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

#ifndef LATBUILDER__PARSER__COMMAND_LINE_H
#define LATBUILDER__PARSER__COMMAND_LINE_H

#include "latbuilder/Types.h"
#include "latbuilder/Task/Search.h"

namespace LatBuilder { namespace Parser {

/**
 * Collection of arguments required to construct a Search instance.
 */
template <LatBuilder::LatType>
struct CommandLine;

/**
 * Specialization of CommandLine for ordinary lattices.
 */
template <>
struct CommandLine<LatBuilder::LatType::ORDINARY> {
   std::string construction;
   std::string size;
   std::string dimension;
   std::string normType;
   std::string figure;
   std::vector<std::string> weights;
   Real weightsPowerScale;
   std::vector<std::string> filters;

   std::unique_ptr<LatBuilder::Task::Search<LatBuilder::LatType::ORDINARY>> parse() const;
};

/**
 * Specialization of CommandLine for embedded lattices.
 */
template <>
struct CommandLine<LatBuilder::LatType::EMBEDDED> : CommandLine<LatBuilder::LatType::ORDINARY> { // **XX
   std::vector<std::string> multilevelFilters;
   std::string combiner;

   std::unique_ptr<LatBuilder::Task::Search<LatBuilder::LatType::EMBEDDED>> parse() const;
};

extern template struct CommandLine<LatBuilder::LatType::ORDINARY>; // **XX
extern template struct CommandLine<LatBuilder::LatType::EMBEDDED>; // **XX

}}

#endif
