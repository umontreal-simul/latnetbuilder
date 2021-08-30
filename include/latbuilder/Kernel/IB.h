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

#ifndef LATBUILDER__KERNEL__IB_H
#define LATBUILDER__KERNEL__IB_H

#include "latbuilder/Kernel/Base.h"
#include "latbuilder/Kernel/FunctorAdaptor.h"
#include "latbuilder/Functor/IB.h"
#include "latbuilder/Util.h"

#include "latticetester/PODWeights.h"

namespace LatBuilder { namespace Kernel {

class IB : public FunctorAdaptor<Functor::IB> {
public:
   IB(unsigned int interlacingFactor):
	  FunctorAdaptor<Functor>(Functor(interlacingFactor))
   {}

   unsigned int interlacingFactor() const
   {
       return functor().interlacingFactor();
   }

    /**
     * This class mimicks the LatticeTester::ProductWeights class.
     * It is used to interlace weights of dimension \f$s\f$ into weights
     * of dimension \f$ d s \f$. See Corollary 3. and Remark 1. of \cite rGOD13a.
     * In the case of the \f$\B_{d, \gamma, (2)}\f$, the weights equal \f$ 2^{-l}\f$ for the
     * \f$l\f$-th interlaced component. This corresponds to \f$\delta_l \f$ in
     * the interlaced weights. @see LatBuilder::Interlaced::IPDWeights.
     */ 
   struct CorrectionProductWeights{
       
    CorrectionProductWeights(const IB& kernel):
        m_interlacingFactor(kernel.interlacingFactor())
    {}

    double getWeight(const LatticeTester::Coordinates & projection) const{
        if (projection.empty())
            return 0;

        double w = 1.0;
        LatticeTester::Coordinates::const_iterator it = projection.begin();
        while (it != projection.end()) {
            w *= getWeightForCoordinate(*it);
            ++it;
        }
        return w;
    }

    double getWeightForCoordinate (LatticeTester::Coordinates::size_type coordinate) const  {
        return intPow(.5,((coordinate + 1) % m_interlacingFactor) + 1);
    }

    private:
        unsigned int m_interlacingFactor;

   };

    /**
     * Corrects POD weights in dimension \f$s\f$. In the case of this kernel, there is nothing to do. See
     * Theorem 3. in \cite rGOD13a.
     * This corresponds to \f$\Gamma\f$ in
     * the interlaced weights. @see LatBuilder::Interlaced::IPDWeights
     */ 
    void correctPODWeights(LatticeTester::PODWeights& weights) const{
        return;
    }

    static constexpr Real CUPower = 1;

};

}}

#endif
