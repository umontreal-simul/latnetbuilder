// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
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

#ifndef LATBUILDER__TASK__EXTEND_H
#define LATBUILDER__TASK__EXTEND_H

#include "latbuilder/Task/Search.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/MeritSeq/LatSeqOverCBC.h"
#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/LatSeq/Combiner.h"
#include "latbuilder/GenSeq/Extend.h"
#include "latbuilder/GenSeq/VectorCreator.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/LatDef.h"

namespace LatBuilder { namespace Task {

template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO, class FIGURE>
class Extend;


/// Exhaustive search.
template <class FIGURE, Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO>
Extend<LR, LAT, COMPRESS, PLO, FIGURE> extend(
      Storage<LR, LAT, COMPRESS, PLO> storage,
      LatDef<LR, LAT> baseLat,
      FIGURE figure
      )
{ return Extend<LR, LAT, COMPRESS, PLO, FIGURE>(std::move(storage), std::move(baseLat), std::move(figure)); }


/**
 * Search task that extends the number of points of a lattice.
 *
 * \tparam LAT, COMPRESS Type of storage.
 * \tparam FIGURE Type of figure of merit.
 */
template <Lattice LR, LatType LAT, Compress COMPRESS, PerLvlOrder PLO, class FIGURE>
class Extend : public Search<LR, LAT> {
public:
   typedef LatBuilder::Storage<LR, LAT, COMPRESS, PLO> Storage;
   typedef typename CBCSelector<LR, LAT, COMPRESS, PLO, FIGURE>::CBC CBC;
   typedef typename CBC::FigureOfMerit FigureOfMerit;
   typedef typename Storage::SizeParam SizeParam;

   Extend(
         Storage storage,
         LatDef<LR, LAT> baseLat,
         FigureOfMerit figure
         ):
      Search<LR, LAT>(baseLat.dimension()),
      m_storage(std::move(storage)),
      m_figure(new FigureOfMerit(std::move(figure))),
      m_latSeqOverCBC(new MeritSeq::LatSeqOverCBC<CBC>(CBC(this->storage(), this->figureOfMerit()))),
      m_baseLat(std::move(baseLat))
   { connectCBCProgress(this->cbc(), this->minObserver(), this->filters().empty()); }

   Extend(Extend&& other):
      Search<LR, LAT>(std::move(other)),
      m_storage(std::move(other.m_storage)),
      m_figure(other.m_figure.release()),
      m_latSeqOverCBC(other.m_latSeqOverCBC.release()),
      m_baseLat(std::move(other.m_baseLat))
   {}

   virtual ~Extend() {}

   virtual void execute()
   {
      typedef GenSeq::Extend<LR> GenSeqType;
      typedef LatSeq::Combiner<LR, LAT, GenSeqType, CartesianProduct> LatSeqType;

      std::vector<GenSeqType> gens(this->dimension());
      gens[0] = GenSeqType(LatticeTraits<LR>::TrivialModulus, LatticeTraits<LR>::TrivialModulus, typename LatticeTraits<LR>::GenValue(1));
      for (size_t j = 1; j < gens.size(); j++)
         gens[j] = GenSeqType(
               storage().sizeParam().modulus(),
               baseLat().sizeParam().modulus(),
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
   const LatDef<LR, LAT>& baseLat() const
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
      std::string name = FIGURE::evaluationName() + " extension of the number of points";
      os << "construction: " << name << std::endl;
      os << "figure of merit: " << figureOfMerit() << std::endl;
      os << "base lattice: " << baseLat() << std::endl;
      os << "size parameter: " << storage().sizeParam() << std::endl;
      Search<LR, LAT>::format(os);
   }

private:
   Storage m_storage;
   std::unique_ptr<FigureOfMerit> m_figure;
   std::unique_ptr<MeritSeq::LatSeqOverCBC<CBC>> m_latSeqOverCBC;
   LatDef<LR, LAT> m_baseLat;
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE1, Extend, NOTAG);

}}

#endif
