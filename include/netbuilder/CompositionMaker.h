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

#include <vector>
#include <utility>

class CompositionMaker
{
    public:

        CompositionMaker(unsigned int n, unsigned int nb_parts);

        bool goToNextComposition();

        const std::pair<std::pair<int,int>, std::pair<int,int>>& changeFromPreviousComposition() const ;

        const std::vector<unsigned int>& currentComposition() const ;

    private:
        unsigned int m_nbParts;
        std::vector<unsigned int> m_composition;
        int m_deepestBoxIndex;
        std::vector<int> m_boxBegin;
        std::vector<int> m_boxEnd;
        std::vector<int> m_master;
        std::pair<std::pair<int,int>, std::pair<int,int>> m_lastChange;

};