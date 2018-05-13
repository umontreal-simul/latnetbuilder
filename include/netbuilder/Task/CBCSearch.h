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

#include "netbuilder/Task/Search.h"

namespace NetBuilder { namespace Task {

/** Class for CBC Search tasks.
 * Template template parameter EXPLORER must implement the following:
 * 
 */ 
template < NetConstruction NC, template <NetConstruction> class EXPLORER>
class CBCSearch : public Search<NC>
{
    public:
        typedef EXPLORER<NC> Explorer;

        /** Constructor.
         * @param dimension is the dimension of the searched net
         * @param nRows is the number of rows of the generating matrices
         * @param nCols is the number of columns of the generating matrices
         * @param figure is a std::unique_ptr to the figure of merit to use
         * @param explorer is a std::unique_ptr to the explorer to use in the search
         */ 
        CBCSearch(  Dimension dimension, 
                    typename NetConstructionTraits<NC>::DesignParameter designParameter,
                    std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
                    std::unique_ptr<Explorer> explorer = std::make_unique<Explorer>(),
                    int verbose = 0 ):
            Search<NC>(dimension, designParameter, std::move(figure), verbose),
            m_explorer(std::move(explorer))
        {
            m_explorer->setVerbose(this->m_verbose-1);
        };

        /** Default move constructor. */
        CBCSearch(CBCSearch&&) = default;

        /** Default destructor. */
        ~CBCSearch() = default;

        /**
        * Executes the search task.
        * The best net and merit value are set in the process.
        */
        virtual void execute() override
        {

            auto evaluator = this->m_figure->evaluator();

            evaluator->onProgress().connect(boost::bind(&MinObserver<NC>::onProgress, &this->minObserver(), _1));
            evaluator->onAbort().connect(boost::bind(&MinObserver<NC>::onAbort, &this->minObserver(), _1));

            Real merit = 0;

            for(unsigned int dim = 1; dim <= this->dimension(); ++dim)
            {
                if(this->m_verbose==1)
                {
                    std::cout << "CBC dimension: " << dim << "/" << this->dimension() << std::endl;
                }

                auto net = this->m_minObserver->bestNet();
                while(!m_explorer->isOver(dim))
                {
                    auto newNet = net.extendDimension(m_explorer->nextGenValue(dim));
                    double newMerit = (*evaluator)(*newNet,dim,merit, this->m_verbose-3);
                    this->m_minObserver->observe(std::move(newNet),newMerit);
                }
                if (!this->m_minObserver->hasFoundNet())
                {
                    this->onFailedSearch()(*this);
                    return;
                }
                merit = this->m_minObserver->bestMerit();
                if (dim < this->dimension()){
                    this->m_minObserver->reset();
                }
            }
            this->selectBestNet(this->m_minObserver->bestNet(), this->m_minObserver->bestMerit());
        }

        virtual void reset() override
        {
            Search<NC>::reset();
            m_explorer.reset();
        }

    private:
        std::unique_ptr<Explorer> m_explorer;
};

}}


#endif
