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

#ifndef LATBUILDER__FUNCTOR__MIN_ELEMENT
#define LATBUILDER__FUNCTOR__MIN_ELEMENT

#include "latbuilder/Functor/AllOf.h"

#include <boost/signals2.hpp>

namespace LatBuilder { namespace Functor {

/**
 * Minimum element functor.
 *
 * Re-implementation of std::min_element that emits a MinElement::onMinUpdated() signal
 * when the current minimum value is updated.
 */
template <typename T>
struct MinElement {
public:
   typedef boost::signals2::signal<void ()> OnStart;
   typedef boost::signals2::signal<void ()> OnStop;
   typedef boost::signals2::signal<void (const T&)> OnMinUpdated;
   typedef boost::signals2::signal<bool (const T&), Functor::AllOf> OnElementVisited;

   /**
    * Constructor.
    */
   MinElement():
      m_onStart(new OnStart),
      m_onStop(new OnStop),
      m_onMinUpdated(new OnMinUpdated),
      m_onElementVisited(new OnElementVisited)
   {}

   /**
    * Returns an iterator pointing to the minimum element between \c first and
    * \c last (exclusively).
    * A start and a stop signals are emitted before the search begins and after
    * it ends, respectively.
    * A minimum-updated signal is emitted when the minimum is updated.
    * An element-visited signal is emitted after an element has been visited.
    */
   template <typename ForwardIterator>
   ForwardIterator operator()(ForwardIterator first, ForwardIterator last) const
   {
      onStart()();

      if (first == last) {
         onStop()();
         return last;
      }

      auto min = *first; // avoid using *itmin
      ForwardIterator itmin = first;
      onMinUpdated()(min);

      if (!onElementVisited()(*first)) {
         onStop()();
         return itmin;
      }

      while (++first != last) {
         if (*first < min) {
            min = *first;
            itmin = first;
            this->onMinUpdated()(min);
         }
         if (!onElementVisited()(*first)) {
            onStop()();
            return itmin;
         }
      }

      onStop()();

      return itmin;
   }

   /**
    * Start signal.
    *
    * Emitted before the search begins.
    */
   OnStart& onStart()
   { return *m_onStart; }

   const OnStart& onStart() const
   { return *m_onStart; }

   /**
    * Stop signal.
    *
    * Emitted after the search ends.
    */
   OnStop& onStop()
   { return *m_onStop; }

   const OnStop& onStop() const
   { return *m_onStop; }

   /**
    * Minimum-updated signal.
    *
    * Emitted when the current minimum value has been updated.
    */
   const OnMinUpdated& onMinUpdated() const
   { return *m_onMinUpdated; }

   OnMinUpdated& onMinUpdated()
   { return *m_onMinUpdated; }

   /**
    * Element-visited updated signal.
    *
    * Emitted after an element is visited.
    */
   OnElementVisited& onElementVisited()
   { return *m_onElementVisited; }

   const OnElementVisited& onElementVisited() const
   { return *m_onElementVisited; }

private:
   std::unique_ptr<OnStart> m_onStart;
   std::unique_ptr<OnStop> m_onStop;
   std::unique_ptr<OnMinUpdated> m_onMinUpdated;
   std::unique_ptr<OnElementVisited> m_onElementVisited;
};

}}

#endif
