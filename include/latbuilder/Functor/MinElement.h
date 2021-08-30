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

#ifndef LATBUILDER__FUNCTOR__MIN_ELEMENT
#define LATBUILDER__FUNCTOR__MIN_ELEMENT

#include "latbuilder/Functor/AllOf.h"

#include <limits>
#include <boost/signals2.hpp>
#include <iostream>

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
   typedef boost::signals2::signal<void (const size_t&)> OnStart;
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
   ForwardIterator operator()(ForwardIterator first, ForwardIterator last, size_t maxAcceptedCount, int verbose = 0) const
   {
      if (maxAcceptedCount == std::numeric_limits<size_t>::max()){
        onStart()( std::distance(first, last));
      }
      else{
        onStart()(maxAcceptedCount);
      }

      if (first == last) {
         onStop()();
         return last;
      }

      auto min = *first; // avoid using *itmin
      if (verbose > 0){
        std::cout << "Current merit: " << *first << " (best) with lattice:" << std::endl;
        std::cout << *first.base().base() << std::endl;
      }
      ForwardIterator itmin = first;
      onMinUpdated()(min);

      if (!onElementVisited()(*first)) {
         onStop()();
         return itmin;
      }

      while (++first != last) {
         bool updated = false;
         if (*first < min) {
            min = *first;
            itmin = first;
            this->onMinUpdated()(min);
            updated = true;
         }

         if (verbose > 0){
            if (updated) {
              std::cout << "Current merit: " << *first << " (best) with lattice:" << std::endl;
            }
            else{
              std::cout << "Current merit: " << *first << " (rejected) with lattice:" << std::endl;
            }
            std::cout << *first.base().base() << std::endl;
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
