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

#ifndef LATBUILDER__TASK__CBC_BASED_SEARCH_H
#define LATBUILDER__TASK__CBC_BASED_SEARCH_H

#include "latbuilder/Task/Search.h"

#include "latbuilder/Types.h"
#include "latbuilder/Storage.h"
#include "latbuilder/MeritFilterList.h"

namespace LatBuilder { namespace Task {

/**
 * Traits for CBCBasedSearch.
 *
 * A specialization for the CBCBasedSearchTraits class
 * must define the \c Storage and \c CBC types, together with the static
 * \c init(), \c genSeqs() and \c name() functions.
 */
template <class>
struct CBCBasedSearchTraits;

/**
 * Component-by-component search task.
 *
 * \tparam TAG Tag class.
 */
template <class TAG>
class CBCBasedSearch : public CBCBasedSearchTraits<TAG>::Search {
public:
   typedef CBCBasedSearchTraits<TAG> Traits;
   typedef typename Traits::Storage Storage;
   typedef typename Traits::CBC CBC;
   typedef typename CBC::FigureOfMerit FigureOfMerit;

   CBCBasedSearch(
         Storage storage,
         Dimension dimension,
         FigureOfMerit figure,
         Traits traits = Traits()
         ):
      CBCBasedSearchTraits<TAG>::Search(dimension),
      m_storage(std::move(storage)),
      m_figure(new FigureOfMerit(std::move(figure))),
      m_cbc(new CBC(this->storage(), this->figureOfMerit())),
      m_traits(std::move(traits))
   { m_traits.init(*this); }

   CBCBasedSearch(CBCBasedSearch&& other) :
      CBCBasedSearchTraits<TAG>::Search(std::move(other)),
      m_storage(std::move(other.m_storage)),
      m_figure(other.m_figure.release()),
      m_cbc(other.m_cbc.release()),
      m_traits(std::move(other.m_traits))
   {}

   virtual ~CBCBasedSearch() {}

   virtual void reset()
   {
      CBCBasedSearchTraits<TAG>::Search::reset();
      m_cbc->reset();
   }

   virtual void execute()
   {
      auto genSeqs = m_traits.genSeqs(storage().sizeParam(), this->dimension());
      this->setObserverTotalDim(this->dimension());

      // iterate through dimension
      for (const auto& genSeq : genSeqs) {
         auto seq = cbc().meritSeq(genSeq);
         auto fseq = this->filters().apply(seq);
         const auto itmin = this->minElement()(fseq.begin(), fseq.end(), this->minObserver().maxAcceptedCount(), this->verbose());
         cbc().select(itmin.base());
         this->selectBestLattice(cbc().baseLat(), *itmin, false);
      }
   }

   /**
    * Returns a pointer to the storage configuration instance.
    */
   const Storage& storage() const
   { return m_storage; }

   /**
    * Returns the figure of merit.
    */
   const FigureOfMerit& figureOfMerit() const
   { return *m_figure; }

   /**
    * Returns the internal CBC instance.
    */
   CBC& cbc()
   { return *m_cbc; }

protected:
   virtual void format(std::ostream& os) const
   {
      os << m_traits.name() << std::endl;
      CBCBasedSearchTraits<TAG>::Search::format(os);
      os << "Modulus: " << storage().sizeParam() << std::endl;
      os << "Figure of merit: " << figureOfMerit() << std::endl;
   }

private:
   Storage m_storage;
   std::unique_ptr<FigureOfMerit> m_figure;
   std::unique_ptr<CBC> m_cbc;
   Traits m_traits;
};

}}

#endif
