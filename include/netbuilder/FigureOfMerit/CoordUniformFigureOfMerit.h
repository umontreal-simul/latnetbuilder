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

#ifndef NETBUILDER__FIGURE_OF_MERIT_BIT__COORD_UNIFORM_FIGURE_OF_MERIT_H
#define NETBUILDER__FIGURE_OF_MERIT_BIT__COORD_UNIFORM_FIGURE_OF_MERIT_H

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"

#include "latticetester/Weights.h"

#include "latbuilder/Kernel/Base.h"
#include "latbuilder/Types.h"
#include "latbuilder/Storage.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/ClonePtr.h"
#include "latbuilder/MeritSeq/CoordUniformStateCreator.h"
#include "latbuilder/MeritSeq/CoordUniformInnerProd.h"

namespace NetBuilder{ namespace FigureOfMerit { 

    /** 
     * Class which represents a coordinate uniform figure of merit based on a kernel which is the template
     * parameter. 
     */  
    template <class KERNEL, LatBuilder::PointSetType PST>
    class CoordUniformFigureOfMerit : public FigureOfMerit
    {
        public:
            /**
            * Constructor.
            *
            * @param weights    See LatBuilder::WeightedFigureOfMerit::WeightedFigureOfMerit for
            *                   details about this parameter.
            * @param sizeParam  Digital size param corresponding to the size of the nets to be evaluated.
            * @param kernel     Kernel (\f$\omega\f$ in the reference paper).  See
            *                   the LatBuilder::Kernel namespace for examples.
            * @param combiner   Combiner used to combine multilevel merits into a single value merit. Not used is PST is UNILEVEL.
            */
            CoordUniformFigureOfMerit(  std::unique_ptr<LatticeTester::Weights> weights, 
                                        LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, PST> sizeParam,
                                        KERNEL kernel = KERNEL(),
                                        Combiner combiner = Combiner()
            ):
            m_weights(std::move(weights)),
            m_kernel(std::move(kernel)),
            m_sizeParam(sizeParam),
            m_combiner(std::move(combiner))
            {}

            /// \copydoc LatBuilder::FigureOfMerit::weights()
            const LatticeTester::Weights& weights() const
            { return *m_weights; }


            /**
            * Returns the coordinate-uniform kernel.
            */
            const KERNEL& kernel() const
            { return m_kernel; }

            /**
            * Returns the coordinate-uniform kernel.
            */
            const LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, PST> sizeParam() const 
            { return m_sizeParam; }

            /** 
             * Returns the name of the figure of merit
             */ 
            std::string name() const
            { return "CU:" + kernel().name(); }

            /**
             * Returns a bool indicating whether the kernel is symmetric.
             */ 
            bool symmetric() const
            { return kernel().symmetric(); }

            /**
             * Returns the suggested compression for this kernel.
             */ 
            static constexpr LatBuilder::Compress suggestedCompression()
            { return KERNEL::suggestedCompression(); }

            /**  
             * Overloads of << operator to print the name of the coord unif merit on the given output stream
             */ 
            friend std::ostream& operator<<(std::ostream& os, const CoordUniformFigureOfMerit& cu_merit)
            {
            os << "Coord unif merit: " << cu_merit.name();
            return os;
            }

            /**
             * Returns a std::unique_ptr to an evaluator for the figure of merit. 
             */
            virtual std::unique_ptr<FigureOfMeritEvaluator> evaluator() override
            {
                return std::make_unique<CoordUniformFigureOfMeritEvaluator>(this);
            }

            /**
             * Creates a new accumulator.
             * @param initialValue Initial accumulator value.
             */
            virtual Accumulator accumulator(Real initialValue) const override
            { return Accumulator(std::move(initialValue), 2); }


            private:

                std::unique_ptr<LatticeTester::Weights> m_weights;
                KERNEL m_kernel;
                LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, PST> m_sizeParam;
                Combiner m_combiner;

                /** 
                 * Class which describes how the figure of merit is computed. 
                 */
                class CoordUniformFigureOfMeritEvaluator : public FigureOfMeritEvaluator
                {
                    template <LatBuilder::LatticeType LR, LatBuilder::Compress COMPRESS >
                    using CoordUniformStateList = std::list<LatBuilder::ClonePtr<LatBuilder::MeritSeq::CoordUniformState<LR, PST, COMPRESS, LatBuilder::defaultPerLevelOrder<LR, PST>::Order>>>;

                    public:

                        /** 
                         * Constructs the evaluator.
                         */
                        CoordUniformFigureOfMeritEvaluator(CoordUniformFigureOfMerit* figure):
                            m_figure(figure),
                            m_storage(LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, PST> {m_figure->sizeParam()}),
                            m_innerProd(m_storage, m_figure->kernel()),
                            m_memStates(LatBuilder::MeritSeq::CoordUniformStateCreator::create(m_innerProd.internalStorage(), m_figure->weights())),
                            m_tmpStates(LatBuilder::MeritSeq::CoordUniformStateCreator::create(m_innerProd.internalStorage(), m_figure->weights()))
                        {};


                        /**
                         * Returns the states.
                         */
                        const CoordUniformStateList<LatBuilder::LatticeType::DIGITAL, KERNEL::suggestedCompression()>& states() const
                        { 
                            return m_memStates; 
                        }

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

                        /**     
                         * Resets the evaluator and prepare it to evaluate a new net.
                         */ 
                        virtual void reset() override
                        {
                            m_memStates = LatBuilder::MeritSeq::CoordUniformStateCreator::create(m_innerProd.internalStorage(), m_figure->weights());
                            m_tmpStates = LatBuilder::MeritSeq::CoordUniformStateCreator::create(m_innerProd.internalStorage(), m_figure->weights());
                        }

                        /** 
                         * Helper overloaded function.
                         */ 
                        MeritValue combine(Real& merit) const 
                        {
                            return merit;
                        }

                        /** 
                         * Returns a combined merit from a vector of merits using the combiner of the figure.
                         */ 
                        MeritValue combine(RealVector& merit) const 
                        {
                            return m_figure->m_combiner(merit);
                        }


                        /** 
                         * Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
                         * starting from the initial value \c initialValue.
                         *  @param net Net to evaluate.
                         *  @param dimension Dimension to compute.
                         *  @param initialValue Initial value of the merit.
                         *  @param verbose Verbosity level.
                         */ 
                        virtual MeritValue operator() (const DigitalNet& net, unsigned int dimension, MeritValue initialValue, int verbose = 0) override
                        {
                            using namespace LatticeTester;

                            MeritValue acc = initialValue; // create the accumulator from the initial value

                            lastMatrix = net.generatingMatrix(dimension);
                            // std::cout << M << std::endl;
                            std::vector<GeneratingMatrix> genSeq {lastMatrix};
                            auto prodSeq = m_innerProd.prodSeq(genSeq, weightedState());
                            // std::cout << "2" << std::endl;
                            auto merit = *(prodSeq.begin());
                            // std::cout << "1" << std::endl;
                            // merit /= intPow(2, M.nCols());
                            m_figure->sizeParam().normalize(merit);
                            acc += combine(merit);

                            if (! onProgress()(acc)) // if someone is listening, may tell that the computation is useless
                            { 
                                acc = std::numeric_limits<Real>::infinity(); // set the merit to infinity
                                onAbort()(net); // abort the computation
                            }
                            return acc;
                        }

                        /**
                         * Tells the evaluator that no more net will be evaluate for the current dimension,
                         * store information about the best net for the dimension which is over and prepare data structures
                         * for the next dimension.
                         */ 
                        virtual void prepareForNextDimension() override
                        {
                            m_memStates = m_tmpStates;
                        } 

                        /**
                         * Tells the evaluator that the last net was the best so far and store the relevant information
                         */
                        virtual void lastNetWasBest() override
                        {
                            m_tmpStates = m_memStates;
                            for (auto& state : m_tmpStates)
                            {
                                state->update(m_innerProd.kernelValues(), lastMatrix);
                            }
                        }

                        // CoordUniformFigureOfMeritEvaluator(CoordUniformFigureOfMeritEvaluator&&) = default;

                        // const CoordUniformFigureOfMerit& figure() const
                        // {
                        //     return *m_figure;
                        // }

                    private:

                        CoordUniformFigureOfMerit* m_figure; // pointer to the figure

                        LatBuilder::Storage<LatBuilder::LatticeType::DIGITAL, PST,  KERNEL::suggestedCompression()> m_storage; // storage for the kernel values

                        LatBuilder::MeritSeq::CoordUniformInnerProd<LatBuilder::LatticeType::DIGITAL, PST,  KERNEL::suggestedCompression(), LatBuilder::PerLevelOrder::BASIC > m_innerProd; // used to compute inner products 

                        CoordUniformStateList<LatBuilder::LatticeType::DIGITAL, KERNEL::suggestedCompression()> m_memStates; // states for the best net for the previous dimension
                        CoordUniformStateList<LatBuilder::LatticeType::DIGITAL, KERNEL::suggestedCompression()> m_tmpStates; // states for the best net so far for the current dimension

                        GeneratingMatrix lastMatrix; // last matrix of latets evaluated net for the current dimension

                };

};

}}
#endif
