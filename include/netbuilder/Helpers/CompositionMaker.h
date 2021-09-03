// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

/**
 * \file 
 * This file contains the definition of a class which computes iteratively all the compositons of an integer \f$ n \f$ in 
 * \f$ k \f$ parts.
 */ 

#include <vector>
#include <utility>

/**
 * Generator of all the compositions of an integer in a specific number of parts.
 * 
 * This class can be used to generate iteratively all the compositions of an integer \f$n\f$ in a specific number of parts \f$k\f$, that
 * is all the \f$ k \f$-uple \f$(a_1, ..., a_k)\f$ such that \f$ \sum_{i=1}^k a_i = n \f$. Furthermore, two consecutive compositions only differ 
 * by one unitary operation: only one unit has been transfered from one \f$ a_i\f$ to another \f$ a_j \f$.
 */

class CompositionMaker
{
    public:

        /**
         * Constructs a generator of all the compositions of \c n into \c k parts. 
         */ 
        CompositionMaker(unsigned int n, unsigned int k);

        /** 
         * Change the current composition to the next composition. 
         * Returns false when the generator is depleted and true otherwise.
         * @return A boolean indicating if the generator is depleted.
         */ 
        bool goToNextComposition();

        /** 
         * Returns the difference between the previous compositon and the current composition.
         * @return A pair of pairs of ints. The first pair indicates which \f$a_i\f$ has decreased and its former value, 
         * the second indicates which \f$ a_i \f$ has increased and its new value.
         */ 
        const std::pair<std::pair<int,int>, std::pair<int,int>>& changeFromPreviousComposition() const ;

        /** 
         * Returns the current composition held by the generator.
         */ 
        const std::vector<unsigned int>& currentComposition() const ;

    private:
        unsigned int m_nbParts; // number of parts in the compositions
        std::vector<unsigned int> m_composition; // current composition
        int m_deepestBoxIndex; // index of the unique non-master
        std::vector<int> m_boxBegin; // indexes of the box beginnings
        std::vector<int> m_boxEnd; // indexes of the box ends
        std::vector<int> m_master; // index of the master of the index
        std::pair<std::pair<int,int>, std::pair<int,int>> m_lastChange; // change between the previous and the current composition

};