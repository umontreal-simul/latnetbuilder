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
template <class KERNEL, LatBuilder::PointSetType PST>
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
         LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, PST> sizeparam,
        KERNEL kernel = KERNEL(),
         std::function<Real (const RealVector&)> combiner = std::function<Real (const RealVector&)>()
         ):
      m_weights(std::move(weights)),
      m_kernel(std::move(kernel)),
    //   m_nbCols((unsigned int) sizeparam.log2numPoints()),
      m_sizeParam(sizeparam),
      m_combiner(std::move(combiner))
   {}

   /// \copydoc FigureOfMerit::weights()
   const LatCommon::Weights& weights() const
   { return *m_weights; }

//    unsigned long nbCols() const
//    { return m_nbCols; }

   /**
    * Returns the coordinate-uniform kernel.
    */
   const KERNEL& kernel() const
   { return m_kernel; }

   const LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, PST> sizeParam() const 
   { return m_sizeParam; }

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
//    unsigned int m_nbCols;
   LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, PST> m_sizeParam;
   std::function<Real (const RealVector&)> m_combiner;

   /** Class which describes how the figure of merit is computed. */
    class CoordUniformFigureOfMeritEvaluator : public FigureOfMeritEvaluator
    {
        template <LatBuilder::LatticeType LR, LatBuilder::Compress COMPRESS >
using CoordUniformStateList = std::list<LatBuilder::ClonePtr<LatBuilder::MeritSeq::CoordUniformState<LR, PST, COMPRESS, LatBuilder::defaultPerLevelOrder<LR, PST>::Order>>>;
        // using namespace LatBuilder::MeritSeq;

        public:
            /** Constructs the evaluator */
            CoordUniformFigureOfMeritEvaluator(CoordUniformFigureOfMerit* figure):
                m_figure(figure),
                // m_storage(LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, PST> {(typename LatBuilder::LatticeTraits<LatBuilder::LatticeType::DIGITAL>::Modulus)(intPow(2, m_figure->nbCols()))}),
                m_storage(LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, PST> {m_figure->sizeParam()}),
                m_innerProd(m_storage, m_figure->kernel()),
                m_memStates(LatBuilder::MeritSeq::CoordUniformStateCreator::create(m_innerProd.internalStorage(), m_figure->weights())),
                m_tmpStates(LatBuilder::MeritSeq::CoordUniformStateCreator::create(m_innerProd.internalStorage(), m_figure->weights()))
            {};


           /**
            * Returns the states.
            */
        const CoordUniformStateList<LatBuilder::LatticeType::DIGITAL, KERNEL::suggestedCompression()>& states() const
        { return m_memStates; }

        /**
            * Returns the total weighted state.
            */
        RealVector weightedState() const
        {
            auto it = states().begin();
            if (it == states().end())
                throw std::runtime_error("CoordUniformCBC: empty list of states");
            auto out = (*it)->weightedState();
            while (++it != states().end())
                out += (*it)->weightedState();
            return out;
        }

        virtual void reset(){
            m_currentDim = 0;
            m_memStates = LatBuilder::MeritSeq::CoordUniformStateCreator::create(m_innerProd.internalStorage(), m_figure->weights());
            m_tmpStates = LatBuilder::MeritSeq::CoordUniformStateCreator::create(m_innerProd.internalStorage(), m_figure->weights());
        }

        MeritValue combine(Real& merit) const {
            return merit;
        }

        MeritValue combine(RealVector& merit) const {
            return m_figure->m_combiner(merit);
        }


        /** Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
         * starting from the initial value \c initialValue.
         * @param net is the net for which we compute the merit
         * @param dimension is the dimension for which we want to compute the merit
         * @param initialValue is the value from which to start
         * @param verbose controls the level of verbosity of the computation
         */ 
        virtual MeritValue operator() (const DigitalNet& net, unsigned int dimension, MeritValue initialValue, unsigned int verbose = 0)
        {
            using namespace LatCommon;

            assert(dimension == m_currentDim || dimension == m_currentDim+1);
            if (dimension == m_currentDim+1){
                m_currentDim++;
                m_memStates = m_tmpStates;
            }

            MeritValue acc = initialValue; // create the accumulator from the initial value
            
            GeneratingMatrix M = net.generatingMatrix(dimension);
            // std::cout << M << std::endl;
            std::vector<GeneratingMatrix> genSeq {M};
            auto prodSeq = m_innerProd.prodSeq(genSeq, weightedState());
            // std::cout << "2" << std::endl;
            auto merit = *(prodSeq.begin());
            // std::cout << "1" << std::endl;
            // merit /= intPow(2, M.nCols());
            m_figure->sizeParam().normalize(merit);
            acc += combine(merit);
            if (! onProgress()(acc)){
                acc = std::numeric_limits<Real>::infinity(); // set the merit to infinity
                onAbort()(net); // abort the computation
                return acc;
            }
            
            m_tmpStates = m_memStates;
            for (auto& state : m_tmpStates){
                state->update(m_innerProd.kernelValues(), M);
            }
            return acc;
        } 

        CoordUniformFigureOfMeritEvaluator(CoordUniformFigureOfMeritEvaluator&&) = default;

        // const decltype(m_storage)& storage(){
        //     return m_storage;
        // }

        const CoordUniformFigureOfMerit& figure(){
            return *m_figure;
        }

        private:
            CoordUniformFigureOfMerit* m_figure;

            LatBuilder::Storage<LatBuilder::LatticeType::DIGITAL, PST,  KERNEL::suggestedCompression()> m_storage;            
            
            LatBuilder::MeritSeq::CoordUniformInnerProd<LatBuilder::LatticeType::DIGITAL, PST,  KERNEL::suggestedCompression(), LatBuilder::PerLevelOrder::BASIC > m_innerProd;

            CoordUniformStateList<LatBuilder::LatticeType::DIGITAL, KERNEL::suggestedCompression()> m_memStates;
            CoordUniformStateList<LatBuilder::LatticeType::DIGITAL, KERNEL::suggestedCompression()> m_tmpStates;

            unsigned int m_currentDim=0;
    };

    virtual std::unique_ptr<CoordUniformFigureOfMeritEvaluator> evaluator(int n)
    {
        return std::make_unique<CoordUniformFigureOfMeritEvaluator>(this);
    }
};

}
}
#endif
