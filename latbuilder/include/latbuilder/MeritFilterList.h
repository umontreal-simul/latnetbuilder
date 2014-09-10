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

#ifndef LATBUILDER__MERIT_FILTER_LIST_H
#define LATBUILDER__MERIT_FILTER_LIST_H

#include "latbuilder/BridgeSeq.h"
#include "latbuilder/LatDef.h"
#include "latbuilder/BasicMeritFilter.h"

#include <boost/signals2.hpp>

#include <memory>
#include <list>

namespace LatBuilder {

/**
 * Container class for merit filters.
 */
template <LatType LAT>
class BasicMeritFilterList;

/**
 * Policy class template for MeritFilterList.
 */
template <LatType>
class MeritFilterListPolicy; // **XX

template <LatType>
class MeritFilterList; // **XX

/**
 * Formats and outputs \c filters to \c os.
 */
std::ostream& operator<<(std::ostream&, const BasicMeritFilterList<LatType::ORDINARY>&);
std::ostream& operator<<(std::ostream&, const BasicMeritFilterList<LatType::EMBEDDED>&);

/**
 * Formats and outputs \c filters to \c os.
 */
std::ostream& operator<<(std::ostream& os, const MeritFilterList<LatType::ORDINARY>& filters);
std::ostream& operator<<(std::ostream& os, const MeritFilterList<LatType::EMBEDDED>& filters);


//========================================================================


template <LatType LAT>
class BasicMeritFilterList {
public:

   typedef BasicMeritFilter<LAT> Filter;
   typedef typename Filter::InputMeritValue  MeritValue;
   typedef typename Filter::LatDef           LatDef;
   typedef std::list<std::unique_ptr<Filter>> FilterList;

   /**
    * Type of signal emitted when a merit value is rejected.
    */
   typedef boost::signals2::signal<void (const LatDef&)> OnReject;

   /**
    * Constructor.
    */
   BasicMeritFilterList():
      m_onReject(new OnReject)
   {}

   /**
    * Returns \c true if and only if the list of filters is empty.
    */
   bool empty() const
   { return filters().empty(); }

   /// \name Setup functions
   //@{

   /**
    * Appends a filter for scalar merit values.
    *
    * \param filter Filter function/functor to add.
    */
   void add(std::unique_ptr<Filter> filter)
   { m_filters.push_back(std::move(filter)); }

   //@}

   /**
    * Reject signal.
    *
    * Emitted when the a value in the output sequence is rejected by a filter.
    */
   OnReject& onReject() const
   { return *m_onReject; }

   const FilterList& filters() const
   { return m_filters; }

protected:
   MeritValue apply(MeritValue merit, const LatDef& lat) const;

private:
   std::unique_ptr<OnReject> m_onReject;
   FilterList m_filters;
};

extern template class BasicMeritFilterList<LatType::ORDINARY>;
extern template class BasicMeritFilterList<LatType::EMBEDDED>;



//========================================================================



/**
 * List of filters for merit values.
 *
 * Transforms sequences of merit values element by element.
 * Consits of a stack of filters, applied sequentially to each merit value.
 *
 * Processes merit values received from its input and produces
 * filtered merit values from its output.
 *
 * The implementation for scalar merit values simply passes through a chain of
 * filters.
 *
 * The implementation for multilevel merit values first pass through a chain of
 * embedded merit filters.  Next, it is converted to simple merit values using a
 * combiner.  Then, it passes through a chain of simple merit filters, before it
 * reaches the output.
 *
 * When a candidate lattice is rejected by a filter, the
 * MeritFilterList::onReject() signal is emitted, no further (downstream)
 * filters are applied and the returned merit value is \c
 * std::numeric_limits<Real>::infinity() .
 *
 * \image html MeritFilterList1.svg
 * \image html MeritFilterList2.svg
 *
 * A filter should return \c true when it accepts a value, or \c false if it
 * rejects it.
 *
 * \remark
 * An alternative, and possibly simpler, approach to filters would be to
 * implement each transformation as a distinct bridge sequence.  However, the
 * stacking of transformations would increase the number of indirections.
 * Moreover, the number of bridge sequences stacked over the original sequence
 * of merit values would be unpredictable, so pieces of code that work on
 * sequences of merit values but need to access the original sequence by calling
 * the member function \c base() would not know in advance how many times to
 * call \c base() to trace back the original sequence.  These are the main
 * reasons why there are filters in Lattice Builder.
 */
template <LatType LAT>
class MeritFilterList : public MeritFilterListPolicy<LAT> {
public:
   typedef MeritFilterListPolicy<LAT> Policy;

   /**
    * Output sequence from the filters.
    */
   template <class MERITSEQ>
   class Seq :
      public BridgeSeq<
         Seq<MERITSEQ>,
         MERITSEQ,
         Real,
         BridgeIteratorCached>
   {
      typedef Seq<MERITSEQ> self_type;

   public:

      typedef typename self_type::Base Base;
      typedef typename self_type::value_type value_type;
      typedef typename self_type::size_type size_type;

      /**
       * Constructor.
       *
       * \param parent     Reference to the parent.  Kept as a reference, no
       *                   copy made.
       * \param base       Original sequence of merit values.
       */
      Seq(const MeritFilterList& parent, Base base):
         self_type::BridgeSeq_(std::move(base)), m_parent(parent)
      {}

      /**
       * Computes and returns the output value of the filters for the basic merit
       * value pointed to by \c it.
       */
      value_type element(const typename Base::const_iterator& it) const
      { return element(it, it); }

   private:
      const MeritFilterList& m_parent;

      template <class IT>
      value_type element(const typename Base::const_iterator& it, const IT& it2) const
      { return element(it, it2, *it2); }

      template <class IT, typename T>
      value_type element(const typename Base::const_iterator& it, const IT& it2, const T&) const
      { return element(it, it2.base()); }

      template <class IT, LatType L>
      value_type element(const typename Base::const_iterator& it, const IT& it2, const LatDef<L>&) const
      { return m_parent.applyFilters(*it, *it2); }
   };

   /**
    * Applies the filters to the input sequence \c meritSeq of merit values.
    *
    * \return A new sequence based on this filters with \c meritSeq as its
    * input.
    *
    * \tparam MERITSEQ  Type of sequence of merit values at the source of the
    *                   filters.
    */
   template <class MERITSEQ>
   Seq<MERITSEQ> apply(MERITSEQ meritSeq) const
   { return Seq<MERITSEQ>(*this, std::move(meritSeq)); }
};


/**
 * Specialization of MeritFilterListPolicy for ordinary lattices.
 */
template <>
class MeritFilterListPolicy<LatType::ORDINARY> :
   public BasicMeritFilterList<LatType::ORDINARY> {

   typedef BasicMeritFilterList<LatType::ORDINARY> OBase;

public:
   template <LatType L>
   typename OBase::OnReject& onReject() const
   { return OBase::onReject(); }

protected:
   Real applyFilters(Real merit, const LatDef& lat) const;
};


/**
 * Specialization of MeritFilterListPolicy for embedded lattices.
 */
template <>
class MeritFilterListPolicy<LatType::EMBEDDED> :
   public BasicMeritFilterList<LatType::ORDINARY>,
   public BasicMeritFilterList<LatType::EMBEDDED> {

   typedef BasicMeritFilterList<LatType::ORDINARY> OBase;
   typedef BasicMeritFilterList<LatType::EMBEDDED> EBase;

public:

   typedef BasicMeritFilter<LatType::EMBEDDED, LatType::ORDINARY> Combiner;

   /**
    * Returns \c true if and only if the list of filters is empty.
    *
    * The list of filters is considered empty when both lists of scalar and
    * multilevel filters are empty.  The combiner is ignored.
    */
   bool empty() const
   { return OBase::empty() and EBase::empty(); }

   /// \name Setup functions
   //@{

   /**
    * Sets the combiner for multilevel merit values.
    *
    * Clears any previously configured combiner.
    */
   void add(std::unique_ptr<Combiner> combiner)
   { m_combiner = std::move(combiner); }

   void add(std::unique_ptr<OBase::Filter> filter)
   { OBase::add(std::move(filter)); }

   void add(std::unique_ptr<EBase::Filter> filter)
   { EBase::add(std::move(filter)); }

   //@}

   template <LatType L>
   typename BasicMeritFilterList<L>::OnReject& onReject() const
   { return BasicMeritFilterList<L>::onReject(); }

   const Combiner& combiner() const
   { 
      if (not m_combiner)
         throw std::runtime_error("no combiner has been specified");
      return *m_combiner;
   }

protected:
   Real applyFilters(const RealVector& merit, const typename EBase::LatDef& lat) const;

private:
   std::unique_ptr<Combiner> m_combiner;
};

}

#endif
