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

#include "netbuilder/Helpers/CompositionMaker.h"

CompositionMaker::CompositionMaker(unsigned int n, unsigned int k):
    m_nbParts(k),
    m_composition(m_nbParts,0),
    m_deepestBoxIndex(0),
    m_boxBegin(m_nbParts),
    m_boxEnd(m_nbParts),
    m_master(m_nbParts)
{
    m_composition[0] = n - (m_nbParts-1);
    for(unsigned int i = 1; i < m_nbParts; ++i)
    {
        m_composition[i] = 1;
    }
    if (m_composition[0] % 2 == 0)
    {
        m_master[0] = - 1;
        m_boxBegin[0] = m_nbParts-1;
        m_boxEnd[0] = 1;

        m_master[m_nbParts-1] = 0;
        m_boxBegin[m_nbParts-1] = m_nbParts-2;
        m_boxEnd[m_nbParts-1] = 1;

        for(unsigned int i = m_nbParts-2; i > 0; --i)
        {
            m_master[i] = i + 1;
            m_boxBegin[i] = i - 1;
            m_boxEnd[i] = 1;
        }

        m_deepestBoxIndex = 1;
    }
    else
    {
        for(unsigned int i = 0 ; i <= m_nbParts-1; ++i)
        {
            m_master[i] = i - 1;
            m_boxBegin[i] = i + 1;
            m_boxEnd[i] = m_nbParts-1;
        }
        m_deepestBoxIndex = m_nbParts-1;
    }
}

bool CompositionMaker::goToNextComposition()
{
    int index = m_master[m_deepestBoxIndex];
    int count = 0;
    while( m_composition[index] == 1 && index > 0 )
    {
        index = m_master[index];
        count++;
    }

    if (index==0 && m_composition[index] == 1)
    {
        return false;
    }

    m_lastChange.first = std::make_pair(index+1, (int) m_composition[index]);
    m_composition[index]--;
    
    std::swap(m_boxBegin[index], m_boxEnd[index]);

    int nextIndex = m_boxBegin[index];
    m_composition[nextIndex]++;

    m_lastChange.second = std::make_pair(nextIndex+1, (int) m_composition[nextIndex]);

    m_master[nextIndex] = index;
    index = nextIndex;

    while (count > 0)
    {
        count--;
        int previousIndex = m_master[index];

        int a, b;
        if (index == previousIndex + 1 || (index < previousIndex && ! (index == previousIndex-1)))
        {
            a = index + 1;
            b = m_boxEnd[previousIndex] ;
            if (m_composition[index] % 2 == 0)
            {
                m_boxBegin[index] = b;
                m_boxEnd[index] = a;
            }
            else
            {
                m_boxBegin[index] = a;
                m_boxEnd[index] = b;
            }
        }
        else
        {
            a = m_boxEnd[previousIndex] ;
            b = index - 1;
            if (m_composition[index] % 2 == 0)
            {
                m_boxBegin[index] = a;
                m_boxEnd[index] = b;
            }
            else
            {
                m_boxBegin[index] = b;
                m_boxEnd[index] = a;
            }
        }

        int nextIndex = m_boxBegin[index];
        m_master[nextIndex] = index;
        index = nextIndex;
    }

    m_boxBegin[index] = -1;
    m_boxEnd[index] = -1;

    m_deepestBoxIndex = index;

    return true;
}

const std::pair<std::pair<int,int>, std::pair<int,int>>& CompositionMaker::changeFromPreviousComposition() const
{
    return m_lastChange;
}

const std::vector<unsigned int>& CompositionMaker::currentComposition() const 
{
    return m_composition;
}
