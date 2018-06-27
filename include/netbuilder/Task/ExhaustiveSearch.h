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

#ifndef NETBUILDER__TASK__EXHAUSTIVE_SEARCH_H
#define NETBUILDER__TASK__EXHAUSTIVE_SEARCH_H

#include "netbuilder/Task/Search.h"

namespace NetBuilder { namespace Task {

/** 
 * Class for exhaustive search tasks.
 */ 
template < NetConstruction NC, EmbeddingType ET>
class ExhaustiveSearch : public Search<NC, ET>
{
    public:
    
        /** Constructor.
         * @param dimension Dimension of the searched net.
         * @param designParameter Design parameter of the searched net.
         * @param figure Figure of merit used to compare nets.
         * @param verbose Verbosity level.
         * @param earlyAbortion Early-abortion switch. If true, the computations will be stopped if the net is worse than the best one so far.
         */
        ExhaustiveSearch(   Dimension dimension, 
                            typename NetConstructionTraits<NC>::DesignParameter designParameter,
                            std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
                            int verbose = 0,
                            bool earlyAbortion = true):
            Search<NC, ET>(dimension, designParameter, std::move(figure), verbose, earlyAbortion)
        {};

        /** 
         * Default move constructor. 
         * Deletes the implicit copy constructor.
         */
        ExhaustiveSearch(ExhaustiveSearch&&) = default;

        /**
        * Executes the search task.
        * The best net and merit value are set in the process.
        */
        virtual void execute() override 
        {

            auto evaluator = this->m_figure->evaluator();

            
            if (this->m_earlyAbortion)
            {
                evaluator->onProgress().connect(boost::bind(&MinimumObserver<NC>::onProgress, &this->minimumObserver(), _1));
                evaluator->onAbort().connect(boost::bind(&MinimumObserver<NC>::onAbort, &this->minimumObserver(), _1));
            }
            
            auto searchSpace = DigitalNetConstruction<NC>::ConstructionMethod::genValueSpace(this->dimension(), this->m_designParameter);
            
            uInteger nbNets = 1;
            for(const auto& genVal : searchSpace)
            {
                if(this->m_verbose>0)
                {
                    std::cout << "Net " << nbNets << "/" << searchSpace.size() << std::endl;
                }
                nbNets++;
                auto net = std::make_unique<DigitalNetConstruction<NC>>(this->m_dimension, this->m_designParameter, genVal);
                double merit = (*evaluator)(*net, this->m_verbose-3);
                this->m_minimumObserver->observe(std::move(net),merit);
            }
            if (!this->m_minimumObserver->hasFoundNet())
            {
                this->onFailedSearch()(*this);
                return;
            }
            this->selectBestNet(this->m_minimumObserver->bestNet(), this->m_minimumObserver->bestMerit());
        }
};

}}


#endif
