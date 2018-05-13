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

#ifndef NETBUILDER__TASK__EXHAUSTIVE_SEARCH_H
#define NETBUILDER__TASK__EXHAUSTIVE_SEARCH_H

#include "netbuilder/Task/Search.h"

namespace NetBuilder { namespace Task {

/** Class for CBC Search tasks.
 * Template template parameter EXPLORER must implement the following:
 * 
 */ 
template < NetConstruction NC>
class ExhaustiveSearch : public Search<NC>
{
    public:
    
        /** Constructor.
         * @param dimension is the dimension of the searched net
         * @param nRows is the number of rows of the generating matrices
         * @param nCols is the number of columns of the generating matrices
         * @param figure is a std::unique_ptr to the figure of merit to use
         */ 
        ExhaustiveSearch(   Dimension dimension, 
                            typename NetConstructionTraits<NC>::DesignParameter designParameter,
                            std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
                            int verbose = 0 ):
            Search<NC>(dimension, designParameter, std::move(figure), verbose)
        {};

        /** Default move constructor. */
        ExhaustiveSearch(ExhaustiveSearch&&) = default;

        /**
        * Executes the search task.
        * The best net and merit value are set in the process.
        */
        virtual void execute() override 
        {

            auto evaluator = this->m_figure->evaluator();

            evaluator->onProgress().connect(boost::bind(&MinObserver<NC>::onProgress, &this->minObserver(), _1));
            evaluator->onAbort().connect(boost::bind(&MinObserver<NC>::onAbort, &this->minObserver(), _1));

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
                this->m_minObserver->observe(std::move(net),merit);
            }
            this->selectBestNet(this->m_minObserver->bestNet(), this->m_minObserver->bestMerit());
        }
};

}}


#endif
