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

#ifndef NETBUILDER__TASK__RANDOM_SEARCH_H
#define NETBUILDER__TASK__RANDOM_SEARCH_H

#include "netbuilder/Task/Search.h"
#include "latbuilder/LFSR258.h"

namespace NetBuilder { namespace Task {

/** Class for CBC Search tasks.
 * Template template parameter EXPLORER must implement the following:
 * 
 */ 
template < NetConstruction NC, EmbeddingType ET>
class RandomSearch : public Search<NC, ET>
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:
    
        /** Constructor.
         * @param dimension Dimension of the searched net.
         * @param sizeParameter Size parameter of the searched net.
         * @param nbTries Number of nets to evaluated.
         * @param figure Figure of merit used to compare nets.
         * @param verbose Verbosity level.
         * @param earlyAbortion Early-abortion switch. If true, the computations will be stopped if the net is worse than the best one so far.
         */
        RandomSearch(   Dimension dimension, 
                        typename NetConstructionTraits<NC>::SizeParameter sizeParameter,
                        std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
                        unsigned nbTries,
                        int verbose = 0,
                        bool earlyAbortion = true):
            Search<NC, ET>(dimension, sizeParameter, verbose, earlyAbortion),
            m_figure(std::move(figure)),
            m_nbTries(nbTries),
            m_randomGenValueGenerator(this->m_sizeParameter)
        {};
    

        /** 
         * Default move constructor. 
         * Deletes the implicit copy constructor.
         */
        RandomSearch(RandomSearch&&) = default;

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
            
            for(unsigned int attempt = 1; attempt <= m_nbTries; ++attempt)
            {
                if(this->m_verbose>0)
                {
                    std::cout << "Net " << attempt << "/" << m_nbTries << std::endl;
                }
                std::vector<typename ConstructionMethod::GenValue> genVals;
                genVals.reserve(this->dimension());
                for(Dimension dim = 0; dim < this->dimension(); ++dim)
                {
                    auto tmp = m_randomGenValueGenerator(dim);
                    genVals.push_back(std::move(tmp));
                }
                auto net = std::make_unique<DigitalNetConstruction<NC>>(this->m_dimension, this->m_sizeParameter, std::move(genVals));
                double merit = (*evaluator)(*net,this->m_verbose-3);
                this->m_minimumObserver->observe(std::move(net),merit);
            }
            if (!this->m_minimumObserver->hasFoundNet())
            {
                this->onFailedSearch()(*this);
                return;
            }
            this->selectBestNet(this->m_minimumObserver->bestNet(), this->m_minimumObserver->bestMerit());
        }

        /**
         * {@inheritDoc}
         */
        virtual const FigureOfMerit::FigureOfMerit& figureOfMerit() const override
        {
            return *m_figure;
        }

    private:
        std::unique_ptr<FigureOfMerit::FigureOfMerit> m_figure;
        unsigned int m_nbTries;
        typename ConstructionMethod:: template RandomGenValueGenerator <LatBuilder::LFSR258> m_randomGenValueGenerator;
};

}}


#endif
