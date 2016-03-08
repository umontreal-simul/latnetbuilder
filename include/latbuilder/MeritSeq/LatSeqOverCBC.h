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

#ifndef LATBUILDER__MERIT_SEQ__LAT_SEQ_OVER_CBC_H
#define LATBUILDER__MERIT_SEQ__LAT_SEQ_OVER_CBC_H

#include "latbuilder/BridgeSeq.h"
#include "latbuilder/BridgeIteratorCached.h"
#include "latbuilder/Traversal.h"

#include <type_traits>
#include <functional>

namespace LatBuilder { namespace MeritSeq {

/**
 * Sequence of merit values for any sequence of lattice definitions.
 *
 * The computation of the merit value for a given generating vector is achieved
 * by applying the CBC algorithm for each component, but considering only the
 * current generator value instead of all values from a sequence of generator
 * values like in the original CBC algorithm. 
 * 
 * \tparam CBC		Type of CBC algorithm.
 */
template <class CBC>
class LatSeqOverCBC
{
   typedef LatSeqOverCBC<CBC> self_type;

public:
   /**
    * Constructor.
    *
    * \param cbc  Instance of the CBC algorithm to be used.
    */
   LatSeqOverCBC(CBC cbc):
      m_cbc(new CBC(std::move(cbc)))
   {}

   const CBC& cbc() const
   { return *m_cbc; }

   /**
    * Output sequence of merit values.
    *
    * \tparam LATSEQ    Type of sequence of lattice definitions.
    */
   template <class LATSEQ>
   class Seq :
      public BridgeSeq<
         Seq<LATSEQ>,
         LATSEQ,
         typename CBC::value_type,
         BridgeIteratorCached>
   {
      typedef Seq<LATSEQ> self_type;

   public:

      typedef typename self_type::Base Base;
      typedef typename self_type::value_type value_type;
      typedef typename self_type::size_type size_type;

      /**
       * Constructor.
       *
       * \param cbc        Instance of the CBC algorithm.
       * \param base       Base lattice sequence.
       */
      Seq(CBC& cbc, Base base):
         self_type::BridgeSeq_(std::move(base)),
         m_cbc(cbc)
      {}

      /**
       * Computes and returns the value of the figure of merit for the generator
       * value pointed to by \c it.
       */
      value_type element(const typename Base::const_iterator& it) const
      {
         m_cbc.reset();

         if (m_cbc.baseLat().sizeParam() != it->sizeParam())
            throw std::logic_error("inconsistent lattice size");

         for (
               auto genIt = it.base().seqIterators().begin();
               genIt != it.base().seqIterators().end();
               ++genIt
               ) {

            // rebind the base generator sequence to a sequence of unit size
            // starting at the current generator index
            auto genSeq = genIt->seq().rebind(
                  Traversal::Forward(genIt->index(), 1));

            m_cbc.select(m_cbc.meritSeq(genSeq).begin());
         }

         return m_cbc.baseMerit();
      }

   private:
      CBC& m_cbc;
   };

   /**
    * Creates a new sequence of merit values based on a sequence of lattice
    * definitions.
    *
    * \param latSeq    Sequence of lattice definitions.
    */
   template <typename LATSEQ>
   Seq<LATSEQ> meritSeq(LATSEQ latSeq) const
   { return Seq<LATSEQ>(*m_cbc, std::move(latSeq)); }

private:
   std::unique_ptr<CBC> m_cbc;
};

/// Creates a search algorithm on top of a CBC algorithm.
template <class CBC>
LatSeqOverCBC<CBC> latSeqOverCBC(CBC cbc)
{ return LatSeqOverCBC<CBC>(std::move(cbc)); }

}}

#endif
