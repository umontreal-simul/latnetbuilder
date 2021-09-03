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

#ifndef LATBUILDER__TASK__SEARCH_H
#define LATBUILDER__TASK__SEARCH_H

#include "latbuilder/Task/Task.h"
#include "latbuilder/FigureOfMerit.h"
#include "latbuilder/MeritFilterList.h"
#include "latbuilder/Functor/MinElement.h"
#include "latbuilder/Functor/LowPass.h"

// for CBCSelector
#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latbuilder/Storage.h"

// for the connect functions
#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/MeritSeq/CoordUniformCBC.h"

#include <boost/signals2.hpp>

#include <memory>

using namespace std::placeholders;

namespace LatBuilder { namespace Task {

/**
 * Base class for search tasks.
 *
 * \tparam ET   Type of lattice.
 */
template <LatticeType LR, EmbeddingType ET>
class Search : public Task {
public:
   typedef boost::signals2::signal<void (const Search&)> OnLatticeSelected;

   /**
    * Observer of the MinElement functor.
    *
    * It allows for counting accepted/rejected elements, and for optionally
    * truncating the sum over projections if, during its term-by-term
    * evaluation, the partial sum reaches a value superior to the current
    * minimum value.  The default behavior is not to truncate the sum.
    */
   class MinObserver {
   public:
      MinObserver(
            size_t maxAcceptedCount = std::numeric_limits<size_t>::max(),
            size_t maxTotalCount = std::numeric_limits<size_t>::max()
            )
      {
         m_totalDim = 0;
         m_dimension = 0;
         m_verbose=0;
         setMaxAcceptedCount(maxAcceptedCount);
         setMaxTotalCount(maxTotalCount);
         setTruncateSum(false);
         start(0);
      }

      void setMaxAcceptedCount(size_t maxCount)
      { m_maxAcceptedCount = maxCount; }

      void setMaxTotalCount(size_t maxCount)
      { m_maxTotalCount = maxCount; }

      /**
       * Sets the truncate-sum flag to \c value.
       *
       * Defaults to \c false.
       */
      void setTruncateSum(bool value)
      { m_truncateSum = value; }

      void start(const size_t& n_totToBeVisited)
      { stop(); m_dimension++; m_totalCount = 0; m_rejectedCount = 0; m_nTotToBeVisited = n_totToBeVisited;}

      /**
       * Reset the low-pass filter when min-element stops.
       */
      void stop()
      { m_lowPass.setThreshold(std::numeric_limits<Real>::max()); }

      bool visited(const Real& r)
      {
         m_totalCount++;
         if (m_verbose > 0 && ((m_nTotToBeVisited > 100 && m_totalCount % 100 == 0) || (m_totalCount % 10 == 0))){
               if (m_totalDim > 1){
                std::cout << "Coordinate " << m_dimension-1 << "/" << m_totalDim <<  " - lattice ";
               }
               else{
                std::cout << "Lattice ";
               }
              std::cout << m_totalCount << "/" << m_nTotToBeVisited << std::endl;
         }
         return acceptedCount() < maxAcceptedCount() and
            totalCount() < maxTotalCount();
      }

      template <LatticeType LA, EmbeddingType L>
      void reject(const LatDef<LA, L>& lat)
      { m_rejectedCount++; }

      /**
       * Updates the threshold of the low-pass filter with the new observed
       * minimum value.
       */
      void minUpdated(const Real& newMin)
      { m_lowPass.setThreshold(newMin); }

      void setVerbosity(int verbose){
        m_verbose = verbose;
      }

      void setTotalDim(Dimension totalDim){
        m_totalDim = totalDim;
      }

      size_t maxAcceptedCount() const
      { return m_maxAcceptedCount; }

      size_t maxTotalCount() const
      { return m_maxTotalCount; }

      size_t acceptedCount() const
      { return m_totalCount - m_rejectedCount; }

      size_t rejectedCount() const
      { return m_rejectedCount; }

      size_t totalCount() const
      { return m_totalCount; }

      /**
       * Applies the low-pass filter if the truncate-sum flag is on.
       *
       * The low-pass filter is bypassed for embedded lattices.
       */
      bool progress(const Real& merit) const
      { return m_truncateSum ? m_lowPass(merit) : true; }

      /**
       * Does nothing.
       */
      bool progress(const RealVector&) const
      { return true; }

      const Functor::LowPass<Real>& lowPass() const
      { return m_lowPass; }

   private:
      Dimension m_dimension;
      int m_verbose;
      bool m_truncateSum;
      size_t m_maxAcceptedCount;
      size_t m_maxTotalCount;
      size_t m_totalCount;
      size_t m_rejectedCount;
      size_t m_nTotToBeVisited;
      Dimension m_totalDim;

      /**
       * Low-pass filter whose threshold is continuously updated with the
       * current smallest value found by the min-element finder.
       *
       * It is meant to be connected to
       * LatBuilder::WeightedFigureOfMerit::onProgress() to interrupt the
       * term-by-term evaluation of the figure of merit when its partial sum/max is
       * larger than the current minimum observed value.
       */
      Functor::LowPass<Real> m_lowPass;

   };

   Search(Dimension dimension):
      m_onLatticeSelected(new OnLatticeSelected),
      m_dimension(dimension),
      m_bestLat(),
      m_bestMerit(0),
      m_minObserver(new MinObserver()),
      m_verbose(0)
   { connectSignals(); }

   Search(Search&& other):
      Task(std::move(other)),
      m_onLatticeSelected(other.m_onLatticeSelected.release()),
      m_dimension(other.m_dimension),
      m_bestLat(std::move(other.m_bestLat)),
      m_bestMerit(std::move(other.m_bestMerit)),
      m_minObserver(other.m_minObserver.release()),
      m_minElement(std::move(other.m_minElement)),
      m_filters(std::move(other.m_filters)),
      m_verbose(other.m_verbose)
   {}

   virtual ~Search() {}

   /**
    * Returns the dimension.
    */
   Dimension dimension() const
   { return m_dimension; }

   int verbose() const
   { return m_verbose; }
   void setVerbose(int verbose)
   { m_verbose= verbose; }

   /**
    * Returns the filters of merit transformations.
    */
   const MeritFilterList<LR, ET>& filters() const
   { return m_filters; }

   /// \copydoc filters() const
   MeritFilterList<LR, ET>& filters()
   { return m_filters; }

   /**
    * Returns the best lattice found by the search task.
    */
   const LatDef<LR, ET>& bestLattice() const
   { return m_bestLat; }

   /**
    * Returns the best merit value found by the search task.
    */
   Real bestMeritValue() const
   { return m_bestMerit; }

   const Functor::MinElement<Real>& minElement() const
   { return m_minElement; }

   /**
    * Returns the minimum-element observer.
    */
   MinObserver& minObserver()
   { return *m_minObserver; }

   const MinObserver& minObserver() const
   { return *m_minObserver; }

   void setObserverVerbosity(int verbose) const
   {
      m_minObserver->setVerbosity(verbose);
   }

   void setObserverTotalDim(Dimension totalDim) const
   {
      m_minObserver->setTotalDim(totalDim);
   }

   /**
    * Lattice-selected signal.
    *
    * Emitted when a lattice has been selected by the search algorithm.
    */
   OnLatticeSelected& onLatticeSelected()
   { return *m_onLatticeSelected; }

   const OnLatticeSelected& onLatticeSelected() const
   { return *m_onLatticeSelected; }

   virtual const FigureOfMerit& figureOfMerit() const = 0;

   /**
    * Executes the search task.
    *
    * The best lattice and merit value are set in the process.
    */
   virtual void execute() = 0;

   /**
    * Initializes the best lattice and merit value.
    */
   virtual void reset()
   {
      m_bestLat = LatDef<LR, ET>();
      m_bestMerit = 0.0;
   }

protected:

   virtual void format(std::ostream& os) const
   {
      os << "Dimension: " << dimension() << std::endl;
      os << filters() << std::endl;
   }

   /**
    * Selects a new best lattice and emits an OnLatticeSelected signal, if quiet is set to false.
    */
   void selectBestLattice(const LatDef<LR, ET>& lattice, Real merit, bool quiet)
   {
      m_bestLat = lattice;
      m_bestMerit = merit;
      if (! quiet){
        onLatticeSelected()(*this);
      }
   }

private:
   std::unique_ptr<OnLatticeSelected> m_onLatticeSelected;

   Dimension m_dimension;
   LatDef<LR, ET> m_bestLat;
   Real m_bestMerit;
   std::unique_ptr<MinObserver> m_minObserver;
   Functor::MinElement<Real> m_minElement;
   MeritFilterList<LR, ET> m_filters;
   int m_verbose;

   void connectSignals()
   {
      // notify minObserver before minElement visits the first element
      m_minElement.onStart().connect(boost::bind(
               &MinObserver::start,
               &minObserver(),
               boost::placeholders::_1
               ));

      // notify minObserver when minElement visits a new element
      m_minElement.onElementVisited().connect(boost::bind(
               &MinObserver::visited,
               &minObserver(),
               boost::placeholders::_1
               ));

      // notify minObserver when the minimum value is updated
      m_minElement.onMinUpdated().connect(boost::bind(
               &MinObserver::minUpdated,
               &minObserver(),
               boost::placeholders::_1
               ));

      connectSignals(filters());
   }

   void connectSignals(MeritFilterList<LR, EmbeddingType::UNILEVEL>& filters)
   {
      // notify minObserver when the filters rejects an element
      filters.template onReject<EmbeddingType::UNILEVEL>().connect(boost::bind(
               &MinObserver::template reject<LR, EmbeddingType::UNILEVEL>,
               &minObserver(),
               boost::placeholders::_1
               ));
   }

   void connectSignals(MeritFilterList<LR, EmbeddingType::MULTILEVEL>& filters)
   {
      // notify minObserver when the filters rejects an element
      filters.template onReject<EmbeddingType::UNILEVEL>().connect(boost::bind(
               &MinObserver::template reject<LR, EmbeddingType::UNILEVEL>,
               &minObserver(),
               boost::placeholders::_1
               ));
      filters.template onReject<EmbeddingType::MULTILEVEL>().connect(boost::bind(
               &MinObserver::template reject<LR, EmbeddingType::MULTILEVEL>,
               &minObserver(),
               boost::placeholders::_1
               ));
   }
};


/**
 * Selector the proper CBC algorithm, given a figure of merit.
 *
 * \tparam ET          Type of lattice.
 * \tparam COMPRESS     Type of compression.
 * \tparam FIGURE       Type of figure of merit.
 *
 * Supported types of figures of merit: any instance of the
 * WeightedFigureOfMerit or of the CoordUniformFigureOfMerit templates.
 *
 * These traits define:
 * - a CBC type that is a component-by-component merit sequence (either an
 *   instance of MeritSeq::CBC or of MeritSeq::CoordUniformCBC) that is appropriate
 *   for use with the FIGURE type;
 * - an init() function that takes an instance of Task::Search as its argument
 *   and that performs special actions depending on the type of CBC algorithm.
 */
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct CBCSelector;

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class PROJDEP, template <class> class ACC>
struct CBCSelector<LR, ET, COMPRESS, PLO, WeightedFigureOfMerit<PROJDEP, ACC>> {
   typedef MeritSeq::CBC<LR, ET, COMPRESS, PLO, PROJDEP, ACC> CBC;
};

template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class KERNEL>
struct CBCSelector<LR, ET, COMPRESS, PLO, CoordUniformFigureOfMerit<KERNEL>> {
   typedef MeritSeq::CoordUniformCBC<LR, ET, COMPRESS, PLO, KERNEL, MeritSeq::CoordUniformInnerProd> CBC;
};


/**
 * Connects WeightedFigureOfMerit::OnProgress with an Search::MinObserver::progress
 * function and activates Search::MinObserver::setTruncateSum().
 */
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class PROJDEP, template <class> class ACC, class OBSERVER>
void connectCBCProgress(const MeritSeq::CBC<LR, ET, COMPRESS, PLO, PROJDEP, ACC>& cbc, OBSERVER& obs, bool truncateSum) {
   typedef typename Storage<LR, ET, COMPRESS, PLO>::MeritValue MeritValue;
   typedef bool (OBSERVER::*ProgressCallback)(const MeritValue&) const;
   ProgressCallback progress = &OBSERVER::progress;

   // We want to interrupt the evaluation of the figure of merit when it
   // reaches a value larger than that threshold, so we pass the partial
   // sum/max through the low-pass filter.
   //
   // Connect the onProgress signal to the function that checks that the
   // current minimum value is greater than the partial sum/max.
   //
   // NOTE: this doesn't work for embedded lattices.
   cbc.evaluator().onProgress().connect(boost::bind(progress, &obs, boost::placeholders::_1));

   // truncate the sum over projections only if no filters are applied
   // downstream
   obs.setTruncateSum(truncateSum);
}

/**
 * Does nothing.
 */
template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO, class KERNEL, template <LatticeType, EmbeddingType, Compress, PerLevelOrder> class PROD, class OBSERVER>
void connectCBCProgress(const MeritSeq::CoordUniformCBC<LR, ET, COMPRESS, PLO, KERNEL, PROD>& cbc, OBSERVER& obs, bool truncateSum) {
   // nothing to do with coordinate-uniform CBC
}

}}

#endif
