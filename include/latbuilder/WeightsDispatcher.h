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

#ifndef LATBUILDER__WEIGHTS_DISPATCHER_H
#define LATBUILDER__WEIGHTS_DISPATCHER_H

#include "latbuilder/CombinedWeights.h"

#include "latticetester/Weights.h"
#include "latticetester/ProjectionDependentWeights.h"
#include "latticetester/OrderDependentWeights.h"
#include "latticetester/ProductWeights.h"
#include "latticetester/PODWeights.h"
#include "netbuilder/Interlaced/IPODWeights.h"

namespace LatBuilder {

class WeightsDispatcher {
public:
   /**
    * Calls functor \c F<WEIGHTS> with arguments \c weights, \c args..., where
    * \c WEIGHTS is replaced with the concrete type of \c weights.
    */
   template <template <typename> class F, typename... ARGS>
   static
   typename std::result_of<F<LatticeTester::Weights>(const LatticeTester::Weights&, ARGS&&...)>::type
   dispatch(const LatticeTester::Weights& weights, ARGS&&... args)
   {
      try { return tryDispatch<F, CombinedWeights                      >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatch<F, NetBuilder::Interlaced::IPODWeights<Kernel::AIDNAlpha>      >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatch<F, NetBuilder::Interlaced::IPODWeights<Kernel::BIDN>      >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatch<F, LatticeTester::ProductWeights            >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatch<F, LatticeTester::OrderDependentWeights     >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatch<F, LatticeTester::PODWeights                >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatch<F, LatticeTester::ProjectionDependentWeights>(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatch<F, LatticeTester::Weights                   >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      throw std::runtime_error("unsupported type of weights");
   }

   /**
    * Calls functor \c F<WEIGHTS> with arguments \c weights, \c args..., where
    * \c WEIGHTS is replaced with the concrete type of \c weights.
    */
   template <template <typename> class F, typename... ARGS>
   static
   typename std::result_of<F<LatticeTester::Weights>(std::unique_ptr<LatticeTester::Weights>, ARGS&&...)>::type
   dispatchPtr(std::unique_ptr<LatticeTester::Weights> weights, ARGS&&... args)
   {
      try { return tryDispatchPtr<F, CombinedWeights                      >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatchPtr<F, NetBuilder::Interlaced::IPODWeights<Kernel::AIDNAlpha>      >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatchPtr<F, NetBuilder::Interlaced::IPODWeights<Kernel::BIDN>      >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatchPtr<F, LatticeTester::ProductWeights            >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatchPtr<F, LatticeTester::OrderDependentWeights     >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatchPtr<F, LatticeTester::PODWeights                >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatchPtr<F, LatticeTester::ProjectionDependentWeights>(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      try { return tryDispatchPtr<F, LatticeTester::Weights                   >(weights, std::forward<ARGS>(args)...); } catch (std::bad_cast&) {}
      throw std::runtime_error("unsupported type of weights");
   }
private:
   template <
      template <typename> class F,
      typename WEIGHTS,
      typename... ARGS
      >
   static
   typename std::result_of<F<LatticeTester::Weights>(const LatticeTester::Weights&, ARGS&&...)>::type
   tryDispatch(const LatticeTester::Weights& weights, ARGS&&... args)
   { return F<WEIGHTS>()(dynamic_cast<const WEIGHTS&>(weights), std::forward<ARGS>(args)...); }

   template <
      template <typename> class F,
      typename WEIGHTS,
      typename... ARGS
      >
   static
   typename std::result_of<F<LatticeTester::Weights>(std::unique_ptr<LatticeTester::Weights>&&, ARGS&&...)>::type
   tryDispatchPtr(std::unique_ptr<LatticeTester::Weights>& weights, ARGS&&... args)
   {
      auto pweights = weights.release();
      std::unique_ptr<WEIGHTS> w(dynamic_cast<WEIGHTS*>(pweights));
      if (not w) {
         weights.reset(pweights);
         throw std::bad_cast();
      }
      return F<WEIGHTS>()(
         std::move(w),
         std::forward<ARGS>(args)...
         );
   }
};

}

#endif
