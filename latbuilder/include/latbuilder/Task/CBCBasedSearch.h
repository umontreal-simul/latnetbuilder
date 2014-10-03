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

#ifndef LATBUILDER__TASK__CBC_BASED_SEARCH_H
#define LATBUILDER__TASK__CBC_BASED_SEARCH_H

#include "latbuilder/Task/Search.h"

#include "latbuilder/Types.h"
#include "latbuilder/Storage.h"
#include "latbuilder/MeritFilterList.h"

namespace LatBuilder { namespace Task {

template <class>
struct CBCBasedSearchTraits;

/**
 * Component-by-component search task.
 *
 * \tparam TAG Tag class.
 *
 * A specialization for the CBCBasedSearchTraits class must be available and
 * must define the \c Storage and \c CBC types, together with the static
 * \c init(), \c genSeqs() and \c name() functions.
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

      // iterate through dimension
      for (const auto& genSeq : genSeqs) {
         auto seq = cbc().meritSeq(genSeq);
         auto fseq = this->filters().apply(seq);
         const auto itmin = this->minElement()(fseq.begin(), fseq.end());
         cbc().select(itmin.base());
         this->selectBestLattice(cbc().baseLat(), *itmin);
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
      os << "construction: " << m_traits.name() << std::endl;
      os << "figure of merit: " << figureOfMerit() << std::endl;
      os << "size parameter: " << storage().sizeParam() << std::endl;
      CBCBasedSearchTraits<TAG>::Search::format(os);
   }

private:
   Storage m_storage;
   std::unique_ptr<FigureOfMerit> m_figure;
   std::unique_ptr<CBC> m_cbc;
   Traits m_traits;
};

}}

#endif
