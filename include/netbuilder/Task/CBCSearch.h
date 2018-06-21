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

#ifndef NETBUILDER__TASK__CBC_SEARCH_H
#define NETBUILDER__TASK__CBC_SEARCH_H

#include "netbuilder/Task/Search.h"

namespace NetBuilder { namespace Task {

/** 
 * Class for CBC Search tasks.
 * Template template parameter EXPLORER must implement the following member functions:
 * - <CODE> void setVerbose() </CODE>: set the verbosity level of the explorer.
 * - <CODE> void reset(unsigned int dim) </CODE>: reset to its initial state the explorer.
 * - <CODE> void switchToDimension(unsigned int dim) </CODE>: switch the explorer to dimension \c dim.
 * - <CODE> typename NetConstructionTraits<NC>::GenValue nextGenValue(unsigned int dim) </CODE>: return the next generating value for dimension \c dim.
 * - <CODE> bool isOver(unsigned int dim) </CODE>: indicate whether the exploration of dimension \c dim is over.
 * where NC is the template parameter of EXPLORER.
 */ 
template < NetConstruction NC, template <NetConstruction> class EXPLORER>
class CBCSearch : public Search<NC>
{
    public:
        typedef EXPLORER<NC> Explorer;

        /** Constructor.
         * @param dimension Dimension of the searched net.
         * @param designParameter Design parameter of the searched net.
         * @param figure Figure of merit used to compare nets.
         * @param explorer Explorer to search for nets.
         * @param verbose Verbosity level.
         * @param earlyAbortion Early-abortion switch. If true, the computations will be stopped if the net is worse than the best one so far.
         */
        CBCSearch(  Dimension dimension, 
                    typename NetConstructionTraits<NC>::DesignParameter designParameter,
                    std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
                    std::unique_ptr<Explorer> explorer = std::make_unique<Explorer>(),
                    int verbose = 0,
                    bool earlyAbortion = true):
            Search<NC>(dimension, designParameter, std::move(figure), verbose, earlyAbortion),
            m_explorer(std::move(explorer))
        {
            m_explorer->setVerbose(this->m_verbose-1);
        };

        /** Constructor.
         * @param dimension Dimension of the searched net.
         * @param baseNet Net from which to start the search.
         * @param figure Figure of merit used to compare nets.
         * @param explorer Explorer to search for nets.
         * @param verbose Verbosity level.
         * @param earlyAbortion Early-abortion switch. If true, the computations will be stopped if the net is worse than the best one so far.
         */
        CBCSearch(  Dimension dimension, 
                    std::unique_ptr<DigitalNetConstruction<NC>> baseNet,
                    std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
                    std::unique_ptr<Explorer> explorer = std::make_unique<Explorer>(),
                    int verbose = 0,
                    bool earlyAbortion = true):
            Search<NC>(dimension, std::move(baseNet), std::move(figure), verbose, earlyAbortion),
            m_explorer(std::move(explorer))
        {
            m_explorer->setVerbose(this->m_verbose-1);
        };

        /** 
         *  Default move constructor. 
         *  Deletes the implicit copy constructor.
         */
        CBCSearch(CBCSearch&&) = default;

        /** 
         * Default destructor. 
         */
        ~CBCSearch() = default;

        /**
         * Executes the search task.
         * The best net and merit value are set in the process.
         */
        virtual void execute() override
        {

            auto evaluator = this->m_figure->evaluator(); // create an evaluator

            // compute the merit of the base net is one was provided
            Real merit = 0; 

            for(unsigned int dim = 0; dim < this->minimumObserver().bestNet().dimension(); ++dim)
            {
                evaluator->prepareForNextDimension();
                merit = (*evaluator)(this->minimumObserver().bestNet(), dim, merit) ;
                evaluator->lastNetWasBest();
            }

            if (this->m_earlyAbortion) // if the switch is on, connect the abortion signals of the evaluator to the observer
            {
                evaluator->onProgress().connect(boost::bind(&MinimumObserver<NC>::onProgress, &this->minimumObserver(), _1));
                evaluator->onAbort().connect(boost::bind(&MinimumObserver<NC>::onAbort, &this->minimumObserver(), _1));
            }

            m_explorer->switchToDimension(this->minimumObserver().bestNet().dimension() + 1); // to to the first dimension to explore

            for(unsigned int dim = this->minimumObserver().bestNet().dimension() ; dim < this->dimension(); ++dim) // for each dimension to explore
            {
                evaluator->prepareForNextDimension();
                if(this->m_verbose==1)
                {
                    std::cout << "CBC dimension: " << dim << "/" << this->dimension() << std::endl;
                }

                auto net = this->m_minimumObserver->bestNet(); // base net of the search
                while(!m_explorer->isOver(dim)) // for each generating values provided by the explorer
                {
                    auto newNet = net.extendDimension(m_explorer->nextGenValue(dim));
                    double newMerit = (*evaluator)(*newNet,dim,merit, this->m_verbose-3); // evaluate the net
                    if (this->m_minimumObserver->observe(std::move(newNet),newMerit)) // give it to the observer
                    {
                        evaluator->lastNetWasBest();
                    }
                }
                if (!this->m_minimumObserver->hasFoundNet())
                {
                    this->onFailedSearch()(*this); // fails if the search has failed
                    return;
                }
                ++m_lastDimension;
                merit = this->m_minimumObserver->bestMerit();
                if (dim + 1 < this->dimension()){ // if at least one dimension remains unexplored
                    this->m_minimumObserver->reset();
                    m_explorer->switchToDimension(dim+1);
                }
            }
            this->selectBestNet(this->m_minimumObserver->bestNet(), this->m_minimumObserver->bestMerit());
        }


        /**
         * Resets the search.
         */         
        virtual void reset() override
        {
            Search<NC>::reset();
            m_explorer->reset();
        }

    private:
        std::unique_ptr<Explorer> m_explorer;
        unsigned int m_lastDimension = 0;
};

}}


#endif
