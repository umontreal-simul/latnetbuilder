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

#ifndef NETBUILDER__TASK__MIN_OBSERVER_H
#define NETBUILDER__TASK__MIN_OBSERVER_H

#include "netbuilder/Types.h"
#include "netbuilder/DigitalNet.h"

#include <boost/signals2.hpp>

#include <memory>
#include <limits>

namespace NetBuilder { namespace Task {

/**
* Observer of the best figure of merit
*
* It allows for truncating the figure if, during its term-by-term evaluation, the partial figure 
* reaches a value superior to the current minimum value.
*/
template <NetConstruction NC>
class MinimumObserver 
{
    public:


        virtual ~MinimumObserver() = default;

        /** 
         * Constructor. 
         * @param sizeParameter Size parameter of the searched net.
         * @param verbose Verbosity level.
        */
        MinimumObserver(typename NetConstructionTraits<NC>::SizeParameter sizeParameter, int verbose = 0):
            m_bestNet(new DigitalNet<NC>(0,sizeParameter)),
            m_verbose(verbose)
        {
            reset(false);
        };

        /** 
         * Constructor. 
         * @param baseNet Net from which to start the search.
         * @param verbose Verbosity level.
        */
        MinimumObserver(std::unique_ptr<DigitalNet<NC>> baseNet, int verbose = 0):
            m_verbose(verbose)
        {
            reset(std::move(baseNet));
        };
            
        /** 
         * Initializes the best observed merit value to infinity, 
         * sets the found net flag to \c false.
         * Optionally, resets the starting net to the empty net.
         * @param hard Flag indicating if the starting net must be reset to the empty net.
         */
        virtual void reset(bool hard = true) 
        { 
            m_bestMerit = std::numeric_limits<Real>::infinity();
            m_foundBestNet = false;
            if (hard)
                m_bestNet = std::make_unique<DigitalNet<NC>>(0, m_bestNet->sizeParameter());
        }

        /** 
         * Initializes the best observed merit value to infinity and 
         * set the found net flag to \c false, and the starting net to baseNet. 
         * @param baseNet The net from which the search starts.
         */
        virtual void reset(std::unique_ptr<DigitalNet<NC>> baseNet) 
        { 
            reset(false);
            m_bestNet = std::move(baseNet);
        }

        /** 
         * Returns the best observed net. 
         */
        const DigitalNet<NC>& bestNet() { return *m_bestNet; }
        
        /** 
         * Returns the best observed merit value. 
         */
        Real bestMerit() { return m_bestMerit; }

        /** 
         * Notifies the observer that the merit value of a new candidate net has
         * been observed, updates the best observed candidate net if necessary.
         */
        virtual bool observe(std::unique_ptr<DigitalNet<NC>> net, const Real& merit)
        {
                if (merit < m_bestMerit){
                    m_bestMerit = merit;
                    m_foundBestNet = true;
                    m_bestNet = std::move(net);

                    if (m_verbose>0)
                    {
                        std::cout << "Current merit: " << merit << " (best) with net:" << std::endl;
                        std::cout << m_bestNet->format(OutputStyle::TERMINAL, 1);
                        std::cout << std::endl;
                        std::cout << std::endl;
                    }
                    return true;
                }
                else
                {
                    if (m_verbose>0)
                    {
                        std::cout << "Current merit: " << merit << " (rejected) with net:" << std::endl;
                        std::cout << net->format(OutputStyle::TERMINAL, 1);
                        std::cout << std::endl;
                        std::cout << std::endl;
                    }
                    return false;
                }
        }

        /**
         * Returns whether the search has found a net.
         */ 
        bool hasFoundNet() const { return m_foundBestNet; }

        /**
         * Returns whether the computation of the merit of a net should continue.
         */ 

        bool onProgress(Real merit) const
        { return merit < m_bestMerit; }

        /**
         * Does nothing.
         */ 
        void onAbort(const AbstractDigitalNet& net) const
        {};

        private:
            std::unique_ptr<DigitalNet<NC>> m_bestNet;
            bool m_foundBestNet;
            Real m_bestMerit;
            int m_verbose;
};

}}


#endif
