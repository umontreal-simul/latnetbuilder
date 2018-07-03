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

#ifndef LATBUILDER__WEIGHTED_FIGURE_OF_MERIT_H
#define LATBUILDER__WEIGHTED_FIGURE_OF_MERIT_H

#include "latbuilder/FigureOfMerit.h"
#include "latticetester/CoordinateSets.h"

#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"
#include "latbuilder/Accumulator.h"
#include "latbuilder/ProjDepMerit/Base.h"
#include "latbuilder/Functor/AllOf.h"
#include "latbuilder/Storage.h"

#include <boost/signals2.hpp>

#include <vector>
#include <memory>

namespace LatBuilder
{

template <class FIGURE, LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO = defaultPerLevelOrder<LR, ET>::Order>
class WeightedFigureOfMeritEvaluator;

/**
 * Weighted figure of merit.
 *
 * This class implements generic weighted average-case figures of merit of the
 * form
 * \f[
 *    D(P_n) = \left[ \mathop{\mathcal K}_{\mathfrak u \in \mathcal J}
 *    \left\{ \gamma_{\mathfrak u}^q D_{\mathfrak u}^q(P_n(\mathfrak u)) \right\} \right]^{1/q},
 * \f]
 * where \f$\mathcal K\f$ is an operator that combines the set elements
 * (typically a sum on its elements or the extraction of the maximum element),
 * using the given projection-dependent weights \f$\gamma_{\mathfrak u}\f$ and a
 * projection-dependent figure of merit \f$D_{\mathfrak u}^2\f$, for all projections
 * \f$\mathfrak u\f$ in a given set of coordinate sets \f$\mathcal J\f$.
 *
 * The set of projections under consideration will typically be
 * \f[
 *   \mathcal J = \left\{
 *     \mathfrak u \::\: \mathfrak u \subseteq \{1,\dots,s\}
 *   \right\}
 * \f]
 * for an exhaustive search over lattices of dimension \f$s\f$, or
 * \f[
 *   \mathcal J = \left\{
 *     \mathfrak u \cup \{s\} \::\: \mathfrak u \subseteq \{1,\dots,s\}
 *   \right\}
 * \f]
 * for a CBC search for the \f$s\f$th coordinate of a lattice.
 *
 * \tparam PROJDEP      Type of projection-dependent figure of merit.
 * \tparam ACC          Type of binary operator to use for the accumulator that
 *                      defines the operator \f$\mathcal K\f$ on the set elements.
 *                      For example, Functor::Max or Functor::Sum.
 *
 * \note The WeightedFigureOfMeritEvaluator object returned by the evaluator()
 * function produces a <strong>square</strong> merit value.
 */
template <class PROJDEP, template <typename> class ACC>
class WeightedFigureOfMerit : public FigureOfMerit
{
public:
   /**
    * Constructor.
    *
    * \param normType     Value of \f$q\f$ as in the \f$q\f$-norm taken over all projections.
    * \param weights      Weights \f$\gamma_{\mathfrak u}^q\f$, already raised
    *                     to the power \f$q\f$.
    * \param projdep      Projection-dependent figure of merit (evaluates to a
    *                     square merit value).
    */
   WeightedFigureOfMerit(
         Real normType,
         std::unique_ptr<LatticeTester::Weights> weights,
         PROJDEP projdep = PROJDEP()
         ):
      m_normType(normType),
      m_weights(std::move(weights)),
      m_projDepMerit(std::move(projdep))
   {}

   /// \copydoc FigureOfMerit::symmetric()
   bool symmetric() const
   { return projDepMerit().symmetric(); }

   static constexpr Compress suggestedCompression()
   { return PROJDEP::suggestedCompression(); }

   Real normType() const
   { return m_normType; }

   /// \copydoc FigureOfMerit::weights()
   const LatticeTester::Weights& weights() const
   { return *m_weights; }

   /**
    * Returns the projection-dependent figure of merit \f$D_{\mathfrak u}^2\f$.
    */
   const ProjDepMerit::Base<PROJDEP>& projDepMerit() const
   { return m_projDepMerit; }

   /**
    * Creates a new accumulator.
    *
    * \param initialValue Initial accumulator value.
    */
   template <typename T>
   static Accumulator<ACC, T> accumulator(T initialValue)
   { return Accumulator<ACC, T>(std::move(initialValue)); }

   /**
    * Creates an evaluator for the figure of merit.
    */
   template <LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO>
   WeightedFigureOfMeritEvaluator<WeightedFigureOfMerit, LR, ET, COMPRESS, PLO>
   evaluator(Storage<LR, ET, COMPRESS, PLO> storage) const
   { return WeightedFigureOfMeritEvaluator<WeightedFigureOfMerit, LR, ET, COMPRESS, PLO>(*this, std::move(storage)); }

   std::string name() const
   { return Accumulator<ACC, Real>::name() + ":" + projDepMerit().name(); }

   static std::string evaluationName()
   { return "projection-by-projection"; }

private:
   Real m_normType;
   std::unique_ptr<LatticeTester::Weights> m_weights;
   PROJDEP m_projDepMerit;

   std::ostream& format(std::ostream& os) const
   {
      os << "WeightedFigureOfMerit("
         << "accumulator=" << Accumulator<ACC, Real>::name() << ", "
         << "norm-type=";
      if (Accumulator<ACC, Real>::name() == "max") {
         if (normType() == 1.0)
            os << "inf";
         else
            os << "inf(" << normType() << ")";
      }
      else
         os << normType();
      os << ", "
         << "projDepMerit=" << projDepMerit() << ", "
         << "weights=" << weights()
         << ")";
      return os;
   }

};

/**
 * Evaluator for WeightedFigureOfMerit.
 *
 * Using an evaluator allows for the WeightedFigureOfMerit object to be
 * instantiated without prior knowledge of the storage class to be used during
 * the evaluation.
 */
template <class FIGURE, LatticeType LR, EmbeddingType ET, Compress COMPRESS, PerLevelOrder PLO >
class WeightedFigureOfMeritEvaluator
{
public:
   typedef typename Storage<LR, ET, COMPRESS, PLO>::MeritValue MeritValue;

   typedef boost::signals2::signal<bool (const MeritValue&), Functor::AllOf> OnProgress;
   typedef boost::signals2::signal<void (const LatDef<LR, ET>&)> OnAbort;

   /**
    * Constructor.
    */
   WeightedFigureOfMeritEvaluator(
         const FIGURE& figure,
         Storage<LR, ET, COMPRESS, PLO> storage
         ):
      m_onProgress(new OnProgress),
      m_onAbort(new OnAbort),
      m_figure(figure),
      m_storage(std::move(storage)),
      m_eval(m_figure.projDepMerit().evaluator(m_storage))
   {}

   WeightedFigureOfMeritEvaluator(WeightedFigureOfMeritEvaluator&&) = default;

   /// \name Signals
   //@{
   /**
    * Progress signal.
    *
    * Emitted after each projection-dependent contribution to the weighted
    * figure of merit is processed.
    * The signal argument is the cumulative value of the weighted figure of
    * merit.  If any of the signal slots returns \c false, the computation of
    * the figure of merit will be aborted.
    */
   OnProgress& onProgress() const
   { return *m_onProgress; }

   /**
    * Abort signal.
    *
    * Emitted if the computation of the weighted figure of merit is aborted.
    * The signal argument is the lattice definition for which the computation
    * was aborted.
    */
   OnAbort& onAbort() const
   { return *m_onAbort; }
   //@}

   /**
    * Returns the <strong>square</strong> value of the figure of merit applied
    * to the projections \c projections of the lattice \c lat.
    *
    * \param lat     Lattice for which the figure of merit will be computed.
    * \param projections  Set of projections \f$\mathcal J\f$ (see LatticeTester::CoordinateSets).
    * \param initialValue  Initial value to put in the accumulator.
    */
   template <class CSETS>
   MeritValue operator() (
         const LatDef<LR, ET>& lat,
         const CSETS& projections,
         MeritValue initialValue
         ) const
   {
   //#define DEBUG
      using namespace LatticeTester;
#ifdef DEBUG
      using TextStream::operator<<;
      std::cout << "computing merit for lattice " << lat << std::endl;
      size_t nproj = 0;
#endif

      auto acc = m_figure.accumulator(std::move(initialValue));

      for (auto cit = projections.begin (); cit != projections.end (); ++cit) {

         const Coordinates& proj = *cit;

         if (*proj.rbegin() >= lat.dimension())
            throw std::invalid_argument("WeightedFigureOfMerit: no such projection");

         Real weight = m_figure.weights().getWeight(proj);

         if (weight == 0.0) {
#ifdef DEBUG
            std::cout << "  skipping projection: " << proj << std::endl;
#endif
            continue;
         }
#ifdef DEBUG
         std::cout << "  processing projection: " << proj << std::endl;
         std::cout << "    weight:    " << weight << std::endl;
#endif

         MeritValue merit = m_eval(lat, proj);

#ifdef DEBUG
         std::cout << "    merit:     " << merit << std::endl;
         std::cout << "    weighted:  " << (weight * merit) << std::endl;
#endif

         // divide q by two because the merit is assumed to be a squared value
         acc.accumulate(weight, merit, m_figure.normType() / 2);

         if (!onProgress()(acc.value())) {
            acc.accumulate(std::numeric_limits<Real>::infinity(), merit, m_figure.normType() / 2);
            onAbort()(lat);
#ifdef DEBUG
            std::cout << "    aborting" << std::endl;
#endif
            break;
         }

#ifdef DEBUG
         ++nproj;
#endif
      }

#ifdef DEBUG
      std::cout << "  projections considered: " << nproj << std::endl;
      std::cout << "  final merit: " << acc.value() << std::endl;
#endif

      return acc.value();
   }

private:
   std::unique_ptr<OnProgress> m_onProgress;
   std::unique_ptr<OnAbort> m_onAbort;

   const FIGURE& m_figure;
   Storage<LR, ET, COMPRESS, PLO> m_storage;
   decltype(m_figure.projDepMerit().evaluator(m_storage)) m_eval;
};

}

#endif
