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

#ifndef NETBUILDER__TASK__CBC_SEARCH_H
#define NETBUILDER__TASK__CBC_SEARCH_H

#include "netbuilder/Types.h"

#include "netbuilder/Task/BaseTask.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"

#include <boost/signals2.hpp>

#include <memory>
#include <limits>

namespace NetBuilder { namespace Task {

/** Class for CBC Search tasks.
 * Template template parameter EXPLORER must implement the following:
 * 
 */ 
template < NetConstruction NC, template <NetConstruction> class EXPLORER>
class CBCSearch : public BaseTask 
{
    public:
        typedef EXPLORER<NC> Explorer;
        typedef boost::signals2::signal<void (const CBCSearch&)> OnNetSelected;
        typedef boost::signals2::signal<void (const CBCSearch&, unsigned int)> OnFailedSearch;

        /**
        * Observer of the best figure of merit
        *
        * It allows for truncating the figure if, during its term-by-term evaluation, the partial figure 
        * reaches a value superior to the current minimum value.
        */
        class MinObserver {
            public:

                /** Constructor. 
                 * @param nRows is the number of rows of the nets
                 * @param nCols is the number of columns of the nets
                 * @verbose is the level of verbosity of the observer
                */
                MinObserver(unsigned int nRows, unsigned nCols, unsigned int verbose = 0):
                    m_bestNet( new DigitalNetConstruction<NC>(nRows,nCols)),
                    m_verbose(verbose)
                {
                    reset();
                };
                    
                /** Initializes the best observed merit value to infinity. */
                void reset() 
                { 
                    m_bestMerit = std::numeric_limits<Real>::infinity();
                    m_foundBestNet = false;
                }

                /** Returns the best observed net. */
                const DigitalNetConstruction<NC>& bestNet() { return *m_bestNet; }
                
                /** Returns the best observed merit value. */
                const Real bestMerit() { return m_bestMerit; }

                /** Notifies the observer that the merit value of a new candidate net has
                 *  been observed, updates the best observed candidate net if necessary.
                 */

                void observe(std::unique_ptr<DigitalNetConstruction<NC>> net, Real merit)
                {
                        if (merit < m_bestMerit) {
                            m_bestMerit = merit;
                            m_foundBestNet = true;
                            m_bestNet = std::move(net);

                            if (m_verbose)
                            {
                                std::cout << *net << "\t:\t" << merit;
                                std::cout << " <-- best";
                                std::cout << std::endl;
                            }
                        }
                }

                // Returns a bool indicating whether the search has found a net.
                bool hasFoundNet() { return m_foundBestNet; }

                // Returns a bool indicating whether the computation should continue.
                bool onProgress(Real merit) const
                { return merit < m_bestMerit; }

                // Does nothing.
                void onAbort(const DigitalNet& net) const
                {};

                private:
                    std::unique_ptr<DigitalNetConstruction<NC>> m_bestNet;
                    bool m_foundBestNet;
                    Real m_bestMerit;
                    unsigned int m_verbose;
        };

        /** Constructor.
         * @param dimension is the dimension of the searched net
         * @param nRows is the number of rows of the generating matrices
         * @param nCols is the number of columns of the generating matrices
         * @param figure is a std::unique_ptr to the figure of merit to use
         * @param explorer is a std::unique_ptr to the explorer to use in the search
         */ 
        CBCSearch(  Dimension dimension, 
                    unsigned int nRows, 
                    unsigned int nCols, 
                    std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
                    std::unique_ptr<Explorer> explorer = std::make_unique<Explorer>(),
                    unsigned int verbose = 0 ):
            m_onNetSelected(new OnNetSelected),
            m_onFailedSearch(new OnFailedSearch),
            m_dimension(dimension),
            m_nRows(nRows),
            m_nCols(nCols),
            m_figure(std::move(figure)),
            m_bestNet(nRows,nCols),
            m_bestMerit(std::numeric_limits<Real>::infinity()),
            m_minObserver(new MinObserver(nRows,nCols)),
            m_explorer(std::move(explorer)),
            m_verbose(verbose)
        {};

        /** Constructor.
         * @param dimension is the dimension of the searched net
         * @param m is the size of the (squared) generating matrices
         * @param figure is a std::unique_ptr to the figure of merit to use
         * @param explorer is a std::unique_ptr to the explorer to use in the search
         */ 
        CBCSearch(  Dimension dimension, 
                    unsigned int m,
                    std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
                    std::unique_ptr<Explorer> explorer = std::make_unique<Explorer>()):
            CBCSearch(dimension,m,m,std::move(figure),std::move(explorer))
        {};

        /** Default move constructor. */
        CBCSearch(CBCSearch&&) = default;

        /** Default destructor. */
        ~CBCSearch() = default;

        /** Returns the dimension of the searched net.
        */
        Dimension dimension() const
        { return m_dimension; }

        /** Returns the number of rows of the searched generating matrices. */
        unsigned int nRows() {return m_nRows; }

        /** Returns the number of columns of the searched generating matrices. */
        unsigned int nCols() {return m_nCols; }

        /** Returns the best net found by the search. */
        const DigitalNetConstruction<NC>& bestNet() const
        { return m_bestNet; }

        /** Returns the best merit found by the search. */
        Real bestMeritValue() const
        { return m_bestMerit; }

        /**
        * Returns the best net found by the search task.
        */
        virtual const DigitalNet& netOutput() const
        { return bestNet(); }

        /**
        * Returns the best merit value found by the search task.
        */
        virtual Real meritValueOutput() const
        { return bestMeritValue(); }

        /** Returns a reference to the minimum-element observer. */
        MinObserver& minObserver()
        { return *m_minObserver; }

        /** Returns a const qualified reference the minimum-element observer. */
        const MinObserver& minObserver() const
        { return *m_minObserver; }

        /**
        * Net-selected signal.
        *  Emitted when a net has been selected by the search algorithm.
        */
        OnNetSelected& onNetSelected()
        { return *m_onNetSelected; }

        const OnNetSelected& onNetSelected() const
        { return *m_onNetSelected; }

        /**
        * Failed search signal.
        *  Emitted when no net has been selected by the search algorithm.
        */
        OnFailedSearch& onFailedSearch()
        { return *m_onFailedSearch; }

        const OnFailedSearch& onFailedSearch() const
        { return *m_onFailedSearch; }

        /** Returns a const qualified reference to the figure of merit. */
        const FigureOfMerit::FigureOfMerit& figureOfMerit() const 
        {
               return *m_figure;
        }

        /**
        * Executes the search task.
        * The best net and merit value are set in the process.
        */
        virtual void execute() {

            auto evaluator = m_figure->evaluator();

            evaluator->onProgress().connect(boost::bind(&MinObserver::onProgress, &minObserver(), _1));
            evaluator->onAbort().connect(boost::bind(&MinObserver::onAbort, &minObserver(), _1));

            Real merit = 0;

            for(unsigned int dim = 1; dim <= m_dimension; ++dim)
            {
                if(m_verbose)
                {
                    std::cout << "======================" << std::endl;
                    std::cout << "Dimension: " << dim << std::endl;
                    std::cout << "----------------------" << std::endl;
                }

                auto net = m_minObserver->bestNet();
                while(!m_explorer->isOver(dim))
                {
                    auto newNet = net.extendDimension(m_explorer->nextGenValue(dim));
                    double newMerit = (*evaluator)(*newNet,dim,merit);
                    m_minObserver->observe(std::move(newNet),newMerit);
                }
                if (!m_minObserver->hasFoundNet())
                {
                    onFailedSearch()(*this, dim);
                    return;
                }
                merit = m_minObserver->bestMerit();
                m_minObserver->reset();
            }
            selectBestNet(m_minObserver->bestNet(), m_minObserver->bestMerit());
        }

    private:

        /**
        * Selects a new best net and emits an OnNetSelected signal.
        */
        void selectBestNet(const DigitalNetConstruction<NC> net, Real merit)
        {
            m_bestNet = net;
            m_bestMerit = merit;
            onNetSelected()(*this);
        }

        std::unique_ptr<OnNetSelected> m_onNetSelected;
        std::unique_ptr<OnFailedSearch> m_onFailedSearch;
        Dimension m_dimension;
        unsigned int m_nRows;
        unsigned int m_nCols;
        std::unique_ptr<FigureOfMerit::FigureOfMerit> m_figure;
        DigitalNetConstruction<NC> m_bestNet;
        Real m_bestMerit;
        std::unique_ptr<MinObserver> m_minObserver;
        std::unique_ptr<Explorer> m_explorer;
        unsigned int m_verbose;
};

}}


#endif
