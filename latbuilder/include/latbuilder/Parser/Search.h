// Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
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
template <LatBuilder::LatType LAT>
class Search {
public:

   /**
    * Parses a string specifying a construction method.
    *
    * Example strings: <code>CBC</code>, <code>fast-CBC</code>, <code>random-CBC:30</code>
    *
    * \return A pointer to a Search instance.
    */
   template <class FIGURE>
   static std::unique_ptr<LatBuilder::Task::Search<LAT>> parse(
         const std::string& construction,
         LatBuilder::SizeParam<LAT> size,
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
      return std::unique_ptr<LatBuilder::Task::Search<LAT>>(ptr());
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
         LatBuilder::SizeParam<LAT> size,
         LatBuilder::Dimension dimension,
         FIGURE figure,
         FUNC&& func, ARGS&&... args
         )
   {
      auto storage = createStorage<FIGURE::suggestedCompression()>(std::move(size));

      auto strSplit = splitPair<std::string, std::string>(str, ':');

      if (str == "CBC") {
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

      if (strSplit.first == "explicit") {
         auto genVec = splitCSV<Modulus>(strSplit.second);
         func(Task::eval(std::move(storage), dimension, std::move(figure), genVec), std::forward<ARGS>(args)...);
         return;
      }

      if (strSplit.first == "extend") {
         auto strSplit2 = splitPair<std::string, std::string>(strSplit.second, ':');
         auto sizeParam = Parser::SizeParam::parse<LAT>(strSplit2.first);
         auto genVec = splitCSV<Modulus>(strSplit2.second);
         auto lat = createLatDef(std::move(sizeParam), std::move(genVec));
         func(Task::extend(std::move(storage), std::move(lat), std::move(figure)), std::forward<ARGS>(args)...);
         return;
      }

      throw ParserError("unsupported construction type: " + str);
   }

private:
   template <Compress COMPRESS>
   static Storage<LAT, COMPRESS> createStorage(LatBuilder::SizeParam<LAT> size)
   { return Storage<LAT, COMPRESS>(std::move(size)); }

   struct ToPtr {
      LatBuilder::Task::Search<LAT>* ptr;
      LatBuilder::Task::Search<LAT>* operator()() const
      { return ptr; }
      template <class TASK> void operator()(TASK task)
      { ptr = new TASK(std::move(task)); }
   };
};

extern template class Search<LatBuilder::LatType::ORDINARY>;
extern template class Search<LatBuilder::LatType::EMBEDDED>;

}}

#endif
