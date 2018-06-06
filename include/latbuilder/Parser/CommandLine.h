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

#ifndef LATBUILDER__PARSER__COMMAND_LINE_H
#define LATBUILDER__PARSER__COMMAND_LINE_H

#include "latbuilder/Types.h"
#include "latbuilder/Task/Search.h"

namespace LatBuilder { namespace Parser {

/**
 * Collection of arguments required to construct a Search instance.
 */
template <LatBuilder::LatticeType , LatBuilder::PointSetType>
struct CommandLine;

/**
 * Specialization of CommandLine for ordinary lattices.
 */
template <LatBuilder::LatticeType LR>
struct CommandLine<LR, LatBuilder::PointSetType::UNILEVEL> {
   std::string construction;
   std::string size;
   std::string dimension;
   std::string normType;
   std::string figure;
   std::vector<std::string> weights;
   Real weightsPowerScale = 1.0;
   std::vector<std::string> filters;

   std::unique_ptr<LatBuilder::Task::Search<LR, LatBuilder::PointSetType::UNILEVEL>> parse() const;
};

/**
 * Specialization of CommandLine for embedded lattices.
 */
template <LatBuilder::LatticeType LR>
struct CommandLine<LR, LatBuilder::PointSetType::MULTILEVEL> : CommandLine<LR, LatBuilder::PointSetType::UNILEVEL> {
   std::vector<std::string> multilevelFilters;
   std::string combiner;

   std::unique_ptr<LatBuilder::Task::Search<LR, LatBuilder::PointSetType::MULTILEVEL>> parse() const;
};

/*
extern template struct CommandLine<LatBuilder::LatticeType::ORDINARY, LatBuilder::PointSetType::UNILEVEL>;
extern template struct CommandLine<LatBuilder::LatticeType::ORDINARY, LatBuilder::PointSetType::MULTILEVEL>;
extern template struct CommandLine<LatBuilder::LatticeType::POLYNOMIAL, LatBuilder::PointSetType::UNILEVEL>;
extern template struct CommandLine<LatBuilder::LatticeType::POLYNOMIAL, LatBuilder::PointSetType::MULTILEVEL>;
*/
}}

#endif
