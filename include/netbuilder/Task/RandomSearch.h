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

#ifndef NETBUILDER__TASK__RANDOM_SEARCH_H
#define NETBUILDER__TASK__RANDOM_SEARCH_H

#include "netbuilder/Task/Search.h"

namespace NetBuilder { namespace Task {

/** Class for CBC Search tasks.
 * Template template parameter EXPLORER must implement the following:
 * 
 */ 
template < NetConstruction NC>
class RandomSearch : public Search<NC>
{
    typedef NetConstructionTraits<NC> ConstructionMethod;

    public:
    
        /** Constructor.
         * @param dimension is the dimension of the searched net
         * @param nRows is the number of rows of the generating matrices
         * @param nCols is the number of columns of the generating matrices
         * @param figure is a std::unique_ptr to the figure of merit to use
         */ 
        RandomSearch(   Dimension dimension, 
                        unsigned int nRows, 
                        unsigned int nCols, 
                        std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
                        unsigned nbTries,
                        unsigned int verbose = 0 ):
            Search<NC>(dimension, nRows, nCols, std::move(figure), verbose),
            m_nbTries(nbTries),
            m_randomGenValueGenerator()
        {};

        /** Constructor.
         * @param dimension is the dimension of the searched net
         * @param m is the size of the (squared) generating matrices
         * @param figure is a std::unique_ptr to the figure of merit to use
         * @param explorer is a std::unique_ptr to the explorer to use in the search
         */ 
        RandomSearch(   Dimension dimension, 
                        unsigned int m,
                        std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
                        unsigned int nbTries):
            RandomSearch(dimension,m,m,std::move(figure),nbTries)
        {};

        /** Default move constructor. */
        RandomSearch(RandomSearch&&) = default;

        /**
        * Executes the search task.
        * The best net and merit value are set in the process.
        */
        virtual void execute() override 
        {

            auto evaluator = this->m_figure->evaluator();

            evaluator->onProgress().connect(boost::bind(&MinObserver<NC>::onProgress, &this->minObserver(), _1));
            evaluator->onAbort().connect(boost::bind(&MinObserver<NC>::onAbort, &this->minObserver(), _1));
            for(unsigned int attempt = 0; attempt < m_nbTries; ++attempt)
            {
                std::vector<typename ConstructionMethod::GenValue> genVal;
                genVal.reserve(this->dimension());
                for(unsigned int dim = 1; dim <= this->dimension(); ++dim)
                {
                    genVal.push_back(m_randomGenValueGenerator(dim));
                }

                auto net = std::make_unique<DigitalNetConstruction<NC>>(genVal, this->nRows(), this->nCols());
                double merit = (*evaluator)(*net);
                this->m_minObserver->observe(std::move(net),merit);
            }
            this->selectBestNet(this->m_minObserver->bestNet(), this->m_minObserver->bestMerit());
        }

    private:
        unsigned int m_nbTries;
        typename ConstructionMethod:: template RandomGenValueGenerator <LatBuilder::LFSR258> m_randomGenValueGenerator;
};

}}


#endif
