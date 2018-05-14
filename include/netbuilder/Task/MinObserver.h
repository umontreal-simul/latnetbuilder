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
class MinObserver 
{
    public:

        /** Constructor. 
         * @param nRows is the number of rows of the nets
         * @param nCols is the number of columns of the nets
         * @verbose is the level of verbosity of the observer
        */
        MinObserver(typename NetConstructionTraits<NC>::DesignParameter designParameter, int verbose = 0):
            m_bestNet(new DigitalNetConstruction<NC>(0,designParameter)),
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

                    if (m_verbose>0)
                    {
                        std::cout << m_bestNet->format(OutputFormat::CLI);
                        std::cout << " <-- merit: "<< m_bestMerit << " (best)";
                        std::cout << std::endl;
                    }
                }
                else
                {
                    if (m_verbose>1)
                    {
                        std::cout << net->format(OutputFormat::CLI);
                        std::cout << " <-- rejected";
                        std::cout << std::endl;
                    }
                }
        }

        // Returns a bool indicating whether the search has found a net.
        bool hasFoundNet() const { return m_foundBestNet; }

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
            int m_verbose;
};

}}


#endif
