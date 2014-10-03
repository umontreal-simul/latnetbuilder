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

#ifndef LATBUILDER__TASK__EXTEND_H
#define LATBUILDER__TASK__EXTEND_H

#include "latbuilder/Task/Search.h"
#include "latbuilder/Task/FigureOfMeritTraits.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/MeritSeq/LatSeqOverCBC.h"
#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/Extend.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/LatDef.h"

namespace LatBuilder { namespace Task {

template <LatType LAT, Compress COMPRESS, class FIGURE>
class Extend;


/// Exhaustive search.
template <class FIGURE, LatType LAT, Compress COMPRESS>
Extend<LAT, COMPRESS, FIGURE> extend(
      Storage<LAT, COMPRESS> storage,
      LatDef<LAT> baseLat,
      FIGURE figure
      )
{ return Extend<LAT, COMPRESS, FIGURE>(std::move(storage), std::move(baseLat), std::move(figure)); }


/**
 * Search task that extends the number of points of a lattice.
 *
 * \tparam LAT, COMPRESS Type of storage.
 * \tparam FIGURE Type of figure of merit.
 */
template <LatType LAT, Compress COMPRESS, class FIGURE>
class Extend : public Search<LAT> {
public:
   typedef LatBuilder::Storage<LAT, COMPRESS> Storage;
   typedef typename FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::CBC CBC;
   typedef typename CBC::FigureOfMerit FigureOfMerit;
   typedef typename Storage::SizeParam SizeParam;

   Extend(
         Storage storage,
         LatDef<LAT> baseLat,
         FigureOfMerit figure
         ):
      Search<LAT>(baseLat.dimension()),
      m_storage(std::move(storage)),
      m_figure(new FigureOfMerit(std::move(figure))),
      m_latSeqOverCBC(new MeritSeq::LatSeqOverCBC<CBC>(CBC(this->storage(), this->figureOfMerit()))),
      m_baseLat(std::move(baseLat))
   { FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::init(*this); }

   Extend(Extend&& other):
      Search<LAT>(std::move(other)),
      m_storage(std::move(other.m_storage)),
      m_figure(other.m_figure.release()),
      m_latSeqOverCBC(other.m_latSeqOverCBC.release()),
      m_baseLat(std::move(other.m_baseLat))
   {}

   virtual ~Extend() {}

   virtual void execute()
   {
      typedef GenSeq::Extend<> GenSeqType;
      typedef LatSeq::Combiner<LAT, GenSeqType, CartesianProduct> LatSeqType;

      std::vector<GenSeqType> gens(this->dimension());
      gens[0] = GenSeqType(2, 2, 1);
      for (size_t j = 1; j < gens.size(); j++)
         gens[j] = GenSeqType(
               storage().sizeParam().numPoints(),
               baseLat().sizeParam().numPoints(),
               baseLat().gen()[j]
               );

      LatSeqType latSeq(storage().sizeParam(), std::move(gens));

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
    * Returns the base lattice on which to extend.
    */
   const LatDef<LAT>& baseLat() const
   { return m_baseLat; }

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
      std::string name = FigureOfMeritTraits<LAT, COMPRESS, FIGURE>::prefix() + "extension of the number of points";
      os << "construction: " << name << std::endl;
      os << "figure of merit: " << figureOfMerit() << std::endl;
      os << "base lattice: " << baseLat() << std::endl;
      os << "size parameter: " << storage().sizeParam() << std::endl;
      Search<LAT>::format(os);
   }

private:
   Storage m_storage;
   std::unique_ptr<FigureOfMerit> m_figure;
   std::unique_ptr<MeritSeq::LatSeqOverCBC<CBC>> m_latSeqOverCBC;
   LatDef<LAT> m_baseLat;
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE1, Extend, NOTAG);

}}

#endif
