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

#ifndef LATBUILDER__TASK__LAT_SEQ_BASED_SEARCH_H
#define LATBUILDER__TASK__LAT_SEQ_BASED_SEARCH_H

#include "latbuilder/Task/Search.h"

#include "latbuilder/Types.h"
#include "latbuilder/Storage.h"
#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritSeq/LatSeqOverCBC.h"

namespace LatBuilder { namespace Task {

/**
 * Traits for LatSeqBasedSearch.
 *
 * A specialization for the CBCBasedSearchTraits class
 * must define the \c Storage and \c CBC types, together with the static \c
 * init(), \c latSeq() and \c name() functions.
 */
template <class>
struct LatSeqBasedSearchTraits;

/**
 * Search task based on a sequence of lattices.
 *
 * \tparam TAG Tag class.
 */
template <class TAG>
class LatSeqBasedSearch : public LatSeqBasedSearchTraits<TAG>::Search {
public:
   typedef LatSeqBasedSearchTraits<TAG> Traits;
   typedef typename Traits::Storage Storage;
   typedef typename Traits::CBC CBC;
   typedef typename CBC::FigureOfMerit FigureOfMerit;


   LatSeqBasedSearch(
         Storage storage,
         Dimension dimension,
         FigureOfMerit figure,
         Traits traits = Traits()
         ):
      LatSeqBasedSearchTraits<TAG>::Search(dimension),
      m_storage(std::move(storage)),
      m_figure(new FigureOfMerit(std::move(figure))),
      m_latSeqOverCBC(new MeritSeq::LatSeqOverCBC<CBC>(CBC(this->storage(), this->figureOfMerit()))),
      m_traits(std::move(traits))
   { m_traits.init(*this); }

   LatSeqBasedSearch(LatSeqBasedSearch&& other):
      LatSeqBasedSearchTraits<TAG>::Search(std::move(other)),
      m_storage(std::move(other.m_storage)),
      m_figure(std::move(other.m_figure)),
      m_latSeqOverCBC(other.m_latSeqOverCBC.release()),
      m_traits(std::move(other.m_traits))
   {}

   virtual ~LatSeqBasedSearch() {}
      
   virtual void execute()
   {
      auto latSeq = m_traits.latSeq(storage().sizeParam(), this->dimension());
      this->setObserverTotalDim(1);

      auto fseq = this->filters().apply(latSeqOverCBC().meritSeq(std::move(latSeq)));
      const auto itmin = this->minElement()(fseq.begin(), fseq.end(), this->minObserver().maxAcceptedCount(), this->verbose());
      this->selectBestLattice(*itmin.base().base(), *itmin, true);
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
    * Returns the internal lattice sequence.
    */
   const MeritSeq::LatSeqOverCBC<CBC>& latSeqOverCBC() const
   { return *m_latSeqOverCBC; }

   /**
    * Returns the internal CBC instance.
    */
   const CBC& cbc() const
   { return latSeqOverCBC().cbc(); }

protected:
   virtual void format(std::ostream& os) const
   {
      os << m_traits.name() << std::endl;
      LatSeqBasedSearchTraits<TAG>::Search::format(os);
      os << "Modulus: " << storage().sizeParam() << std::endl;
      os << "Figure of merit: " << figureOfMerit() << std::endl;
   }

private:
   Storage m_storage;
   std::unique_ptr<FigureOfMerit> m_figure;
   std::unique_ptr<MeritSeq::LatSeqOverCBC<CBC>> m_latSeqOverCBC;
   Traits m_traits;
};

}}

#endif
