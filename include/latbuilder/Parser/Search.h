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

#ifndef LATBUILDER__PARSER__SEARCH_H
#define LATBUILDER__PARSER__SEARCH_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Parser/SizeParam.h"
#include "latbuilder/Task/CBC.h"
#include "latbuilder/Task/FastCBC.h"
#include "latbuilder/Task/RandomCBC.h"
#include "latbuilder/Task/Eval.h"
#include "latbuilder/Task/Exhaustive.h"
#include "latbuilder/Task/Random.h"
#include "latbuilder/Task/Korobov.h"
#include "latbuilder/Task/RandomKorobov.h"
#include "latbuilder/Task/Extend.h"
#include "latbuilder/Storage.h"

#include <string>
#include <vector>

namespace LatBuilder { namespace Parser {

/**
 * Parser for coordinate-uniform figures of merit.
 */
template <LatticeType LR, LatBuilder::EmbeddingType ET>
class Search {
public:

   /**
    * Parses a string specifying a construction method.
    *
    * Example strings: <code>full-CBC</code>, <code>fast-CBC</code>, <code>random-CBC:30</code>
    *
    * \return A pointer to a Search instance.
    */
   template <class FIGURE>
   static std::unique_ptr<LatBuilder::Task::Search<LR, ET>> parse(
         const std::string& construction,
         LatBuilder::SizeParam<LR, ET> size,
         LatBuilder::Dimension dimension,
         FIGURE figure
         )
   {
      ToPtr ptr;
      parse(
            construction,
            std::move(size),
            dimension,
            std::move(figure),
            ptr
           );
      return std::unique_ptr<LatBuilder::Task::Search<LR, ET>>(ptr());
   }

   /**
    * Parses a string specifying a construction method.
    * 
    * Calls \c func with the newly created search instance as its first
    * argument, and \c args as additional arguments.
    */
   template <class FIGURE, class FUNC, typename... ARGS>
   static void parse(
         const std::string& str,
         LatBuilder::SizeParam<LR, ET> size,
         LatBuilder::Dimension dimension,
         FIGURE figure,
         FUNC&& func, ARGS&&... args
         )
   {
      const LatBuilder::PerLevelOrder P = defaultPerLevelOrder<LR, ET>::Order;

      auto storage = createStorage< FIGURE::suggestedCompression(), P>(std::move(size));

      auto strSplit = splitPair<std::string, std::string>(str, ':');

      if (str == "full-CBC") {
         func(Task::cbc(std::move(storage), dimension, std::move(figure)), std::forward<ARGS>(args)...);
         return;
      }
      if (str == "fast-CBC") {
         func(Task::fastCBC(std::move(storage), dimension, std::move(figure)), std::forward<ARGS>(args)...);
         return;
      }
      if (str == "Korobov") {
         func(Task::korobov(std::move(storage), dimension, std::move(figure)), std::forward<ARGS>(args)...);
         return;
      }
      if (str == "exhaustive") {
         func(Task::exhaustive(std::move(storage), dimension, std::move(figure)), std::forward<ARGS>(args)...);
         return;
      }

      try {
         auto nrand = boost::lexical_cast<unsigned int>(strSplit.second);
         if (strSplit.first == "random") {
            func(Task::random(std::move(storage), dimension, std::move(figure), nrand), std::forward<ARGS>(args)...);
            return;
         }
         if (strSplit.first == "random-CBC") {
            func(Task::randomCBC(std::move(storage), dimension, std::move(figure), nrand), std::forward<ARGS>(args)...);
            return;
         }
         if (strSplit.first == "random-Korobov") {
            func(Task::randomKorobov(std::move(storage), dimension, std::move(figure), nrand), std::forward<ARGS>(args)...);
            return;
         }
      }
      catch (boost::bad_lexical_cast&) {}

      if (strSplit.first == "evaluation") {
         auto genVec = LatticeParametersParseHelper<LR>::ParseGeneratingVector(strSplit.second);
         func(Task::eval(std::move(storage), dimension, std::move(figure), genVec), std::forward<ARGS>(args)...);
         return;
      }

      if (strSplit.first == "extend") {
         auto strSplit2 = splitPair<std::string, std::string>(strSplit.second, ':');
         auto sizeParam = Parser::SizeParam<LR, ET>::parse(strSplit2.first);
         auto genVec =  LatticeParametersParseHelper<LR>::ParseGeneratingVector(strSplit2.second);
         auto lat = createLatDef(std::move(sizeParam), std::move(genVec));
         func(Task::extend(std::move(storage), std::move(lat), std::move(figure)), std::forward<ARGS>(args)...);
         return;
      }

      throw ParserError("unsupported construction method: " + str);
   }

private:
   template <Compress COMPRESS, PerLevelOrder PLO>
   static Storage<LR, ET, COMPRESS, PLO> createStorage(LatBuilder::SizeParam<LR, ET> size)
   { return Storage<LR, ET, COMPRESS, PLO>(std::move(size)); }

   struct ToPtr {
      LatBuilder::Task::Search<LR, ET>* ptr;
      LatBuilder::Task::Search<LR, ET>* operator()() const
      { return ptr; }
      template <class TASK> void operator()(TASK task)
      { ptr = new TASK(std::move(task)); }
   };
};

extern template class Search<LatticeType::ORDINARY, LatBuilder::EmbeddingType::UNILEVEL>;
extern template class Search<LatticeType::ORDINARY, LatBuilder::EmbeddingType::MULTILEVEL>;
extern template class Search<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::UNILEVEL>;
extern template class Search<LatticeType::POLYNOMIAL, LatBuilder::EmbeddingType::MULTILEVEL>;

}}

#endif
