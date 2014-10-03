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

#ifndef LATBUILDER__TASK__LAT_SEQ_BASED_SEARCH_H
#define LATBUILDER__TASK__LAT_SEQ_BASED_SEARCH_H

#include "latbuilder/Task/Search.h"

#include "latbuilder/Types.h"
#include "latbuilder/Storage.h"
#include "latbuilder/MeritFilterList.h"
#include "latbuilder/MeritSeq/LatSeqOverCBC.h"

namespace LatBuilder { namespace Task {

template <class>
struct LatSeqBasedSearchTraits;

/**
 * Search task based on a sequence of lattices.
 *
 * \tparam TAG Tag class.
 *
 * A specialization for the CBCBasedSearchTraits class must be available and
 * must define the \c Storage and \c CBC types, together with the static \c
 * init(), \c latSeq() and \c name() functions.
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
      m_latSeqOverCBC(std::move(other.m_latSeqOverCBC)),
      m_traits(std::move(other.m_traits))
   {}

   virtual ~LatSeqBasedSearch() {}
      
   virtual void execute()
   {
      auto latSeq = m_traits.latSeq(storage().sizeParam(), this->dimension());

      auto fseq = this->filters().apply(latSeqOverCBC().meritSeq(std::move(latSeq)));
      const auto itmin = this->minElement()(fseq.begin(), fseq.end());
      this->selectBestLattice(*itmin.base().base(), *itmin);
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
      os << "construction: " << m_traits.name() << std::endl;
      os << "figure of merit: " << figureOfMerit() << std::endl;
      os << "size parameter: " << storage().sizeParam() << std::endl;
      LatSeqBasedSearchTraits<TAG>::Search::format(os);
   }

private:
   Storage m_storage;
   std::unique_ptr<FigureOfMerit> m_figure;
   std::unique_ptr<MeritSeq::LatSeqOverCBC<CBC>> m_latSeqOverCBC;
   Traits m_traits;
};

}}

#endif
