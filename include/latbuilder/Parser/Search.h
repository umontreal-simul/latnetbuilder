// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>

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

      std::vector<std::string> strSplit;
      boost::split(strSplit, str, boost::is_any_of(":"));

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
         auto nrand = boost::lexical_cast<unsigned int>(strSplit[1]);
         if (strSplit[0] == "random") {
            func(Task::random(std::move(storage), dimension, std::move(figure), nrand), std::forward<ARGS>(args)...);
            return;
         }
         if (strSplit[0] == "random-CBC") {
            func(Task::randomCBC(std::move(storage), dimension, std::move(figure), nrand), std::forward<ARGS>(args)...);
            return;
         }
         if (strSplit[0] == "random-Korobov") {
            func(Task::randomKorobov(std::move(storage), dimension, std::move(figure), nrand), std::forward<ARGS>(args)...);
            return;
         }
      }
      catch (boost::bad_lexical_cast&) {}

      
      int splitSizeWithoutFile;
      if (strSplit[0] == "evaluation"){
         splitSizeWithoutFile = 2;
      }
      else if (strSplit[0] == "extend"){
         splitSizeWithoutFile = 3;
      }
      else {
         throw ParserError("unsupported construction method: " + str);
      }

      std::string genVecString;
      if (strSplit.size() == splitSizeWithoutFile){
            genVecString = strSplit[splitSizeWithoutFile-1];
      }
      else if (strSplit.size() == splitSizeWithoutFile + 1){
            std::ifstream t(strSplit[splitSizeWithoutFile]);
            std::stringstream buffer;
            buffer << t.rdbuf();
            genVecString = buffer.str();
            std::vector<std::string> fileLines;
            boost::split(fileLines, genVecString, boost::is_any_of("\n"));
            unsigned int firstLineCoordinate;
            for (unsigned int i=0; i<fileLines.size(); i++){
                if (fileLines[i][0] == '#'){
                    firstLineCoordinate = i+1;
                }
            }
            std::vector<std::string> filteredFileLines;
            for (unsigned int i=firstLineCoordinate; i<fileLines.size(); i++){
                filteredFileLines.push_back(fileLines[i]);
            }
            genVecString = boost::algorithm::join(filteredFileLines, "-");
            boost::replace_all(genVecString, " ", ",");
      }
      else {
         throw ParserError("evaluation or exploration parameter is not well specified.");
      }

      if (strSplit[0] == "evaluation") {
         auto genVec = LatticeParametersParseHelper<LR>::ParseGeneratingVector(genVecString);
         func(Task::eval(std::move(storage), dimension, std::move(figure), genVec), std::forward<ARGS>(args)...);
         return;
      }

      if (strSplit[0] == "extend") {
         auto sizeParam = Parser::SizeParam<LR, ET>::parse(strSplit[1]);
         auto genVec =  LatticeParametersParseHelper<LR>::ParseGeneratingVector(genVecString);
         auto lat = createLatDef(std::move(sizeParam), std::move(genVec));
         func(Task::extend(std::move(storage), std::move(lat), std::move(figure)), std::forward<ARGS>(args)...);
         return;
      }

      
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
