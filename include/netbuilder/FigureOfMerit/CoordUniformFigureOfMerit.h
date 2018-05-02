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

#ifndef NETBUILDER__COORD_UNIFORM_FIGURE_OF_MERIT_H
#define NETBUILDER__COORD_UNIFORM_FIGURE_OF_MERIT_H

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"

#include "latcommon/Weights.h"
#include "latcommon/Coordinates.h"
#include "latbuilder/Kernel/Base.h"
#include "latbuilder/Types.h"
#include "latbuilder/Storage.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/ClonePtr.h"
#include "latbuilder/MeritSeq/CoordUniformStateCreator.h"
#include "latbuilder/MeritSeq/CoordUniformInnerProd.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <functional>

namespace NetBuilder{ namespace FigureOfMerit { 

/** Class which represents a coordinate uniform figure of merit based on a kernel which is the template
 * parameter. 
 */  
template <class KERNEL>
class CoordUniformFigureOfMerit : public FigureOfMerit
{
public:
   /**
    * Constructor.
    *
    * \param weights    See WeightedFigureOfMerit::WeightedFigureOfMerit for
    *                   details about this parameter.
    * \param kernel     Kernel (\f$\omega\f$ in the reference paper).  See
    *                   the Kernel namespace for examples.
    */
   CoordUniformFigureOfMerit(
         std::unique_ptr<LatCommon::Weights> weights,
         unsigned int m,
         KERNEL kernel = KERNEL()
         ):
      m_weights(std::move(weights)),
      m_kernel(std::move(kernel)),
      m_nbCols(m)
   {}

   /// \copydoc FigureOfMerit::weights()
   const LatCommon::Weights& weights() const
   { return *m_weights; }

   unsigned long nbCols() const
   { return m_nbCols; }

   /**
    * Returns the coordinate-uniform kernel.
    */
   const KERNEL& kernel() const
   { return m_kernel; }

   std::string name() const
   { return "CU:" + kernel().name(); }

    bool symmetric() const
   { return kernel().symmetric(); }

   static constexpr LatBuilder::Compress suggestedCompression()
   { return KERNEL::suggestedCompression(); }

    /**  Overload of << operator to print the name of the coord unif merit on the given output stream
     */ 
    friend std::ostream& operator<<(std::ostream& os, const CoordUniformFigureOfMerit& cu_merit)
    {
        os << "Coord unif merit: " << cu_merit.name();
        return os;
    }

    /** Instantiates an evaluator for the figure of merit and returns a smart pointer to it.
     */
    virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator()
    {
        return std::make_unique<CoordUniformFigureOfMeritEvaluator>(this);
    }


private:
   std::unique_ptr<LatCommon::Weights> m_weights;
   KERNEL m_kernel;
   unsigned int m_nbCols;

   /** Class which describes how the figure of merit is computed. */
    class CoordUniformFigureOfMeritEvaluator : public FigureOfMeritEvaluator
    {
        // using namespace LatBuilder::MeritSeq;

        public:
            /** Constructs the evaluator */
            CoordUniformFigureOfMeritEvaluator(CoordUniformFigureOfMerit* figure):
                m_figure(figure),
                m_storage(LatBuilder::SizeParam<LatBuilder::LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE> {(typename LatBuilder::LatticeTraits<LatBuilder::LatticeType::ORDINARY>::Modulus)(intPow(2, m_figure->nbCols()))}),
                m_innerProd(m_storage, m_figure->kernel()),
                m_states(LatBuilder::MeritSeq::CoordUniformStateCreator::create(m_innerProd.internalStorage(), m_figure->weights()))
            {};

        /** Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
         * starting from the initial value \c initialValue.
         * @param net is the net for which we compute the merit
         * @param dimension is the dimension for which we want to compute the merit
         * @param initialValue is the value from which to start
         * @param verbose controls the level of verbosity of the computation
         */ 
        virtual MeritValue operator() (const DigitalNet& net, unsigned int dimension, MeritValue initialValue, bool verbose = true)
        {
            using namespace LatCommon;


            return initialValue + 1;
        } 

        CoordUniformFigureOfMeritEvaluator(CoordUniformFigureOfMeritEvaluator&&) = default;

        // const decltype(m_storage)& storage(){
        //     return m_storage;
        // }

        const CoordUniformFigureOfMerit& figure(){
            return *m_figure;
        }

        private:
            template <LatBuilder::LatticeType LR, LatBuilder::LatEmbed LAT, LatBuilder::Compress COMPRESS >
using CoordUniformStateList = std::list<LatBuilder::ClonePtr<LatBuilder::MeritSeq::CoordUniformState<LR, LAT, COMPRESS, LatBuilder::defaultPerLevelOrder<LR, LAT>::Order>>>;

            CoordUniformFigureOfMerit* m_figure;

            CoordUniformStateList<LatBuilder::LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE,  KERNEL::suggestedCompression()> m_states;

            LatBuilder::Storage<LatBuilder::LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE,  KERNEL::suggestedCompression()> m_storage;            
            
            LatBuilder::MeritSeq::CoordUniformInnerProd<LatBuilder::LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE,  KERNEL::suggestedCompression(), LatBuilder::defaultPerLevelOrder<LatBuilder::LatticeType::ORDINARY, LatBuilder::LatEmbed::SIMPLE>::Order > m_innerProd;
    };
};

}
}
#endif
