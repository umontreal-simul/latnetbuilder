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

#ifndef LATBUILDER__INDEX_MAP_H
#define LATBUILDER__INDEX_MAP_H

#include <boost/numeric/ublas/detail/iterator.hpp>
#include <boost/numeric/ublas/exception.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

namespace LatBuilder {

/**
 * Permutation of vector indices.
 *
 * This class is meant to permutations of Boost uBLAS vector object.
 * The simplest way to use this class is through the permuteVector() function.
 *
 * Credits: this class is an adaptation of Boost's indirect array class.
 */
template<class M>
class IndexMap {
    typedef IndexMap<M> self_type;
public:
    typedef M mapper_type;
    typedef const M const_mapper_type;
    typedef typename M::size_type size_type;
    typedef typename M::size_type value_type;
    typedef typename M::size_type difference_type;
    typedef const value_type const_reference;
    typedef value_type reference;

    // Construction and destruction
    BOOST_UBLAS_INLINE
    IndexMap ():
        mapper_ () {}
    explicit BOOST_UBLAS_INLINE
    IndexMap (mapper_type mapper):
        mapper_ (std::move(mapper)) {}

    BOOST_UBLAS_INLINE
    size_type size () const {
        return mapper_.size ();
    }
    BOOST_UBLAS_INLINE
    const_mapper_type& mapper () const {
        return mapper_;
    }
    BOOST_UBLAS_INLINE
    mapper_type& mapper () {
        return mapper_;
    }

    // Random Access Container
    BOOST_UBLAS_INLINE
    size_type max_size () const {
        return mapper_.size ();
    }
    
    BOOST_UBLAS_INLINE
    bool empty () const {
        return mapper_.size () == 0;
    }
        
    // Element access
    BOOST_UBLAS_INLINE
    size_type operator () (size_type i) const {
        BOOST_UBLAS_CHECK (i < size (), boost::numeric::ublas::bad_index ());
        return mapper_ (i);
    }
    BOOST_UBLAS_INLINE
    size_type operator () (size_type i) {
        BOOST_UBLAS_CHECK (i < size (), boost::numeric::ublas::bad_index ());
        return mapper_ (i);
    }

    BOOST_UBLAS_INLINE
    size_type operator [] (size_type i) const {
        return (*this) (i);
    }
    BOOST_UBLAS_INLINE
    size_type operator [] (size_type i) {
        return (*this) (i);
    }

    // Comparison
    template<class OM>
    BOOST_UBLAS_INLINE
    bool operator == (const IndexMap<OM> &ia) const {
        if (size () != ia.size ())
            return false;
        for (size_type i = 0; i < BOOST_UBLAS_SAME (size (), ia.size ()); ++ i)
            if (mapper_ (i) != ia.mapper_ (i))
                return false;
        return true;
    }
    template<class OM>
    BOOST_UBLAS_INLINE
    bool operator != (const IndexMap<OM> &ia) const {
        return not (*this == ia);
    }

public:
    typedef boost::numeric::ublas::indexed_const_iterator<IndexMap, std::random_access_iterator_tag> const_iterator;

    BOOST_UBLAS_INLINE
    const_iterator begin () const {
        return const_iterator (*this, 0);
    }
    BOOST_UBLAS_INLINE
    const_iterator end () const {
        return const_iterator (*this, size ());
    }

    // Reverse iterator
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    BOOST_UBLAS_INLINE
    const_reverse_iterator rbegin () const {
        return const_reverse_iterator (end ());
    }
    BOOST_UBLAS_INLINE
    const_reverse_iterator rend () const {
        return const_reverse_iterator (begin ());
    }
    BOOST_UBLAS_INLINE
    IndexMap preprocess (size_type size) const {
        return *this;
    }

private:
    mapper_type mapper_;
};

/**
 * Returns a vector proxy whose elements are a permutation of another vector.
 *
 * \param vec       Original vector.
 * \param mapper    Object that defines a permutation of vector indices through
 *                  the definition of its member \c operator().
 *
 * To use this function, first define a mapper class, say \c MyMapper, that
 * defines the type \c size_type, that implements \c size() whose return value
 * is number of elements in the permutation, and that implements \c operator()
 * that maps an input index (the operator argument) to an output index, e.g.:
 * \code
 * class MyMapper {
 * public:
 *    typedef unsigned size_type;
 *    size_type size() const { return ...; }
 *    size_type operator() (size_type i) const { return ...; }
 * };
 * \endcode
 * Next, simply instantiate a Boost uBLAS vector \c vec and obtain a
 * permutation \c pvec:
 * \code
 * boost::numeric::ublas::vector<unsigned> vec(10);
 * auto pvec = LatBuilder::permuteVector(vec, MyMapper());
 * \endcode
 */
template <typename VEC, typename MAP>
boost::numeric::ublas::vector_indirect<VEC,IndexMap<MAP>>
permuteVector(VEC& vec, MAP mapper) {
   typedef IndexMap<MAP> IMap;
   return boost::numeric::ublas::vector_indirect<VEC,IMap>(vec, IMap(std::move(mapper)));
}

}

/** \example IndexMap.cc
    This example shows how to make use of the LatBuilder::IndexMap class to permute vector
    elements.
*/

#endif
