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

#ifndef NETBUILDER__SOBOL_DIRECTION_NUMBERS_H
#define NETBUILDER__SOBOL_DIRECTION_NUMBERS_H

#include "latbuilder/Traversal.h"
#include "netbuilder/Util.h"

#include <vector>

namespace NetBuilder {

template <class TRAV = LatBuilder::Traversal::Forward>
class SobolDirectionNumbers :
    public LatBuilder::Traversal::Policy<SobolDirectionNumbers<TRAV>, TRAV> 
{

    typedef SobolDirectionNumbers<TRAV> self_type;
    typedef LatBuilder::Traversal::Policy<self_type, TRAV> TraversalPolicy;

    public:

        /**
        * Size type.
        */
        typedef size_t size_type;

        typedef unsigned long value_type;

        /**
            * Traversal type.
            */
        typedef TRAV Traversal;

        static std::string name()
        { return std::string("Direction numbers / ") + " / " + Traversal::name(); }

        /**
        * Constructor.
        *
        * \param nbBits    Maximal size of the numbers in bits
        * \param trav       Traversal instance.
        */
        SobolDirectionNumbers(unsigned int nbBits, Traversal trav = Traversal());

        /**
        * Cross-traversal copy-constructor.
        */
        template <class TRAV2>
        SobolDirectionNumbers(
                const SobolDirectionNumbers<TRAV2>& other,
                Traversal trav = Traversal()):
            TraversalPolicy(std::move(trav)),
            m_nbBits(other.m_nbBits),
            m_size(other.m_size),
            m_data(other.m_data)
        {}

        /**
        * Rebinds the traversal type.
        */
        template <class TRAV2>
        struct RebindTraversal {
            typedef SobolDirectionNumbers<TRAV2> Type;
        };

        /**
        * Returns a copy of this object, but using a different traversal policy.
        */
        template <class TRAV2>
        typename RebindTraversal<TRAV2>::Type rebind(TRAV2 trav) const
        { return typename RebindTraversal<TRAV2>::Type{*this, std::move(trav)}; }

        /**
        * Returns the maximal number of bits in the direction numbers.
        */
        value_type nbBits() const
        { return m_nbBits; }

        /**
        * Returns the size of the sequence.
        *
        * The size is the value of Euler's totient function.
        */
        size_type size() const
        { return m_size; }

        /**
        * Returns the element at index \c i.
        */
        value_type operator[](size_type i) const;

    private:

        template <class> friend class SobolDirectionNumbers;

        unsigned int m_nbBits;
        size_type m_size;
        std::vector<value_type> m_data;
};

}

//========================================================================
// IMPLEMENTATION
//========================================================================

namespace NetBuilder {

template <class TRAV>
SobolDirectionNumbers<TRAV>::SobolDirectionNumbers(unsigned int nbBits, Traversal trav):
   TraversalPolicy(std::move(trav)),
   m_nbBits(nbBits),
   m_size(intPow(2,nbBits-1)),
   m_data(m_size)
{
    size_type i = 0;
    for(unsigned long k = 1; k < 2*m_size; k+=2)
    {
        m_data[i] = k;
        ++i;
    }
}

template <class TRAV>
auto SobolDirectionNumbers<TRAV>::operator[](size_type i) const -> value_type
{
   return m_data[i];
}

}

#endif
