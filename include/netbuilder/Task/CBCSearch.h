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
 * - <CODE> void reset() </CODE>: reset to its initial state the explorer.
 * - <CODE> void switchToCoordinate(Dimension coord) </CODE>: switch the explorer to coordinate \c coord.
 * - <CODE> typename NetConstructionTraits<NC>::GenValue nextGenValue() </CODE>: return the next generating value.
 * - <CODE> bool isOver() </CODE>: indicate whether the exploration of the current coordinate is over.
 * where NC is the template parameter of EXPLORER.
 */ 
template < NetConstruction NC, EmbeddingType ET, template <NetConstruction, EmbeddingType> class EXPLORER, template <NetConstruction> class OBSERVER = MinimumObserver>
class CBCSearch : public Search<NC, ET, OBSERVER>
{
    public:
        typedef EXPLORER<NC, ET> Explorer;

        /** Constructor.
         * @param dimension Dimension of the searched net.
         * @param sizeParameter Size parameter of the searched net.
         * @param figure Figure of merit used to compare nets.
         * @param explorer Explorer to search for nets.
         * @param verbose Verbosity level.
         * @param earlyAbortion Early-abortion switch. If true, the computations will be stopped if the net is worse than the best one so far.
         */
        CBCSearch(  Dimension dimension, 
                    typename NetConstructionTraits<NC>::SizeParameter sizeParameter,
                    std::unique_ptr<FigureOfMerit::CBCFigureOfMerit> figure,
                    std::unique_ptr<Explorer> explorer = std::make_unique<Explorer>(),
                    int verbose = 0,
                    bool earlyAbortion = false):
            Search<NC, ET, OBSERVER>(dimension, sizeParameter, verbose, earlyAbortion),
            m_figure(std::move(figure)),
            m_explorer(std::move(explorer))
        {};

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
                    std::unique_ptr<FigureOfMerit::CBCFigureOfMerit> figure,
                    std::unique_ptr<Explorer> explorer = std::make_unique<Explorer>(),
                    int verbose = 0,
                    bool earlyAbortion = false):
            Search<NC, ET, OBSERVER>(dimension, std::move(baseNet), verbose, earlyAbortion),
            m_figure(std::move(figure)),
            m_explorer(std::move(explorer))
        {};

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
         *  Returns information about the task
         */
        virtual std::string format() const override
        {
            std::string res;
            std::ostringstream stream;
            stream << Search<NC, ET, OBSERVER>::format();
            stream << "Exploration method: CBC - " << m_explorer->format() << std::endl;
            stream << "Figure of merit: " << m_figure->format() << std::endl;
            res += stream.str();
            stream.str(std::string());
            return res;
        }

        /**
         * Executes the search task.
         * The best net and merit value are set in the process.
         */
        virtual void execute() override
        {

            auto evaluator = this->m_figure->evaluator(); // create an evaluator

            // compute the merit of the base net is one was provided
            Real merit = 0; 

            for(Dimension coord = 0; coord < this->observer().bestNet().dimension(); ++coord)
            {
                evaluator->prepareForNextDimension();
                merit = (*evaluator)(this->observer().bestNet(), coord, merit) ;
                evaluator->lastNetWasBest();
            }

            if (this->m_earlyAbortion) // if the switch is on, connect the abortion signals of the evaluator to the observer
            {
                evaluator->onProgress().connect(boost::bind(&Search<NC, ET, OBSERVER>::Observer::onProgress, &this->observer(), _1));
                evaluator->onAbort().connect(boost::bind(&Search<NC, ET, OBSERVER>::Observer::onAbort, &this->observer(), _1));
            }

            m_explorer->switchToCoordinate(this->observer().bestNet().dimension()); // to to the first dimension to explore

            for(Dimension coord = this->observer().bestNet().dimension() ; coord < this->dimension(); ++coord) // for each dimension to explore
            {
                evaluator->prepareForNextDimension();
                if(this->m_verbose>=1 && coord > 0)
                {
                    std::cout << "Begin coordinate: " << coord + 1 << "/" << this->dimension() << std::endl;
                }
                auto net = this->m_observer->bestNet(); // base net of the search
                while(!m_explorer->isOver()) // for each generating values provided by the explorer
                {
                    auto newNet = net.extendDimension(m_explorer->nextGenValue());
                    unsigned long totalSize = m_explorer->size();
                    if (this->m_verbose>=2 && ((totalSize > 100 && m_explorer->count() % 100 == 0) || (m_explorer->count() % 10 == 0)))
                    {
                        std::cout << "Coordinate " << coord + 1 << "/" << this->dimension() << " - net " << m_explorer->count() << "/" << totalSize << std::endl;
                    }
                    double newMerit = (*evaluator)(*newNet,coord,merit, this->m_verbose-3); // evaluate the net
                    if (this->m_observer->observe(std::move(newNet),newMerit)) // give it to the observer
                    {
                        evaluator->lastNetWasBest();
                    }
                }
                if (!this->m_observer->hasFoundNet())
                {
                    this->onFailedSearch()(*this); // fails if the search has failed
                    return;
                }
                merit = this->m_observer->bestMerit();
                if(this->m_verbose>=1)
                {
                    std::string netExplored;
                    if (m_explorer->size() == 1){
                        netExplored = "1 net";
                    }
                    else{
                        netExplored = std::to_string(m_explorer->size()) + " nets";
                    }
                    std::cout << "End coordinate: " << coord + 1 << "/" << this->dimension() << " - " << netExplored << " explored - partial merit value: " << merit << std::endl;
                }
                if (coord + 1 < this->dimension()){ // if at least one dimension remains unexplored
                    this->m_observer->reset(false);
                    m_explorer->switchToCoordinate(coord+1);
                }
            }
            this->selectBestNet(this->m_observer->bestNet(), this->m_observer->bestMerit());
        }


        /**
         * Resets the search.
         */         
        virtual void reset() override
        {
            Search<NC, ET, OBSERVER>::reset();
            m_explorer->reset();
        }

        /**
         * {@inheritDoc}
         */
        virtual const FigureOfMerit::CBCFigureOfMerit& figureOfMerit() const override
        {
            return *m_figure;
        }

    private:
        std::unique_ptr<FigureOfMerit::CBCFigureOfMerit> m_figure;
        std::unique_ptr<Explorer> m_explorer;
};

}}


#endif
