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

#ifndef LATBUILDER__KERNEL__IC_ALPHA_H
#define LATBUILDER__KERNEL__IC_ALPHA_H

#include <memory>

#include "latbuilder/Kernel/Base.h"
#include "latbuilder/Kernel/FunctorAdaptor.h"
#include "latbuilder/Functor/ICAlpha.h"

#include "latticetester/PODWeights.h"
#include "latticetester/Coordinates.h"

namespace LatBuilder { namespace Kernel {

class ICAlpha : public FunctorAdaptor<Functor::ICAlpha> {
public:
   ICAlpha(unsigned int alpha, unsigned int interlacingFactor):
	  FunctorAdaptor<Functor>(Functor(alpha, interlacingFactor))
   {}

   unsigned int alpha() const
   { return functor().alpha(); }

   unsigned int interlacingFactor() const
   {
       return functor().interlacingFactor();
   }

    /**
     * This class mimicks the LatticeTester::ProductWeights class.
     * It is used to interlace weights of dimension \f$s\f$ into weights
     * of dimension \f$ d s \f$. See Corollary 2. and Remark 1. of \cite rGOD13a.
     * In the case of the \f$\B_{d, \gamma, (1)}\f$, the weights are even equal to 1 for all
     * the interlaced components. This corresponds to \f$\delta_l \f$ in
     * the interlaced weights (see LatBuilder::Interlaced::IPDWeights).
     */ 
   struct CorrectionProductWeights{
       CorrectionProductWeights(const ICAlpha& kernel){}

       double getWeight(const LatticeTester::Coordinates & projection) const{
           return 1;
       }
       double getWeightForCoordinate (LatticeTester::Coordinates::size_type coordinate) const  {
           return 1;
       }
   };

    /**
     * Corrects POD weights in dimension \f$s\f$ using the definition of \f$D_{\alpha, d}\f$ in Section 2.4 of \cite rGOD15c.
     * \f[
     *  \tilde{\gamma}_{\mathfrak u} = D_{\alpha, d}^{|\mathfrak u|} \gamma_{\mathfrak u} = \left(4^{\max(d-\alpha, 0)} 2^{\alpha (2 d - 1)}\right)^{|\mathfrak u|} \gamma_{\mathfrak u}.
     * \f]
     * This corresponds to \f$\Gamma\f$ in
     * the interlaced weights (see LatBuilder::Interlaced::IPDWeights).
     */ 
    void correctPODWeights(LatticeTester::PODWeights& weights) const{
        weights.getProductWeights().multiplyWeights((1 << (alpha() * (2 * interlacingFactor() - 1) + 2 * std::max((int) interlacingFactor() - (int) alpha(), 0))));
    }

    static constexpr Real CUPower = 1;

};

}}

#endif
